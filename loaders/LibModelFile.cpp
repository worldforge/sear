// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: LibModelFile.cpp,v 1.24 2006-05-06 10:47:05 simon Exp $

/*
  Debug check list
  * Correct Indices
  * glDrawElements faces pointer data
  * Correct normal calculation

*/

#include <sigc++/object_slot.h>

extern "C" {
#include <libmd3/structure.h>
#include <libmd3/loader.h>
#include <libmd3/convert.h>
#include <libmd3/mesh.h>
}

#include <sage/sage.h>
#include <sage/GL.h>

#include "common/Utility.h"

#include "src/System.h"
#include "src/FileHandler.h"

#include "renderers/RenderSystem.h"

#include "LibModelFile.h"

#include "StaticObject.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

static const float default_scale = 1.0f / 64.0f;

static const std::string SECTION_model = "model";

static const std::string KEY_filename = "filename";
static const std::string KEY_rotation = "rotation";
static const std::string KEY_scale = "scale";
static const std::string KEY_scale_isotropic = "scale_isotropic";
static const std::string KEY_scale_isotropic_x = "scale_isotropic_x";
static const std::string KEY_scale_isotropic_y = "scale_isotropic_y";
static const std::string KEY_scale_isotropic_z = "scale_isotropic_z";
static const std::string KEY_scale_anisotropic = "scale_anisotropic";
static const std::string KEY_z_align = "z_align";

static const std::string KEY_texture_map_0 = "texture_map_0";
static const std::string KEY_ambient = "ambient";
static const std::string KEY_diffuse = "diffuse";
static const std::string KEY_specular = "specular";
static const std::string KEY_emission = "emission";
static const std::string KEY_shininess = "shininess";

typedef enum {
  AXIS_ALL = 0,
  AXIS_X,
  AXIS_Y,
  AXIS_Z
} Axis;

static void scale_object(LibModelFile::StaticObjectList &objs, Axis axis, bool isotropic, bool z_align) {
  float min[3], max[3];
  bool firstPoint = true;
  // Find bounds of object
  for (LibModelFile::StaticObjectList::const_iterator I = objs.begin(); I != objs.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);

    float m[4][4];
    so->getMatrix().getMatrix(m);
    float *v = so->getVertexDataPtr();
    for (unsigned int i = 0; i < so->getNumPoints(); ++i) {
      float x = v[i * 3 + 0];
      float y = v[i * 3 + 1];
      float z = v[i * 3 + 2];
      float w = 1.0f;

      // Transform points by matrix
      float nx = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3] * w;
      float ny = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3] * w;
      float nz = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3] * w;
      float nw = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3] * w;

      x = nx / nw;
      y = ny / nw;
      z = nz / nw;

      if (firstPoint) {
        firstPoint = false;
        min[0] = max[0] = x;
        min[1] = max[1] = y;
        min[2] = max[2] = z;
      } else {
        if (x < min[0]) min[0] = x;
        if (y < min[1]) min[1] = y;
        if (z < min[2]) min[2] = z;

        if (x > max[0]) max[0] = x;
        if (y > max[1]) max[1] = y;
        if (z > max[2]) max[2] = z;
      }
    }
  }
  // Re-scale all points
  float diff_x = fabs(max[0] - min[0]);
  float diff_y = fabs(max[1] - min[1]);
  float diff_z = fabs(max[2] - min[2]);

  // Isotropic keeps the "aspect ratio" of the model by performing a constant
  // Scaling in all axis.
  // Otherwise each axis is scaled by a different amount
  if (isotropic) {
    switch (axis) {
      case AXIS_X: // Scale so X axis is 1.0
        diff_y = diff_z = diff_x;
        break;
      case AXIS_Y: // Scale so Y axis is 1.0
        diff_x = diff_z = diff_y;
        break;
      case AXIS_Z: // Scale so Z axis is 1.0
        diff_x = diff_y = diff_z;
        break;
      default:
      case AXIS_ALL: // Scale so largest axis is 1.0
        diff_x = diff_y = diff_z = std::max(std::max(diff_x, diff_y), diff_z);
        break;
    }
  }

  float scale_x = 1.0 / (diff_x);
  float scale_y = 1.0 / (diff_y);
  float scale_z = 1.0 / (diff_z);

  for (LibModelFile::StaticObjectList::const_iterator I = objs.begin(); I != objs.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);

    float *v = so->getVertexDataPtr();
    float m[4][4];
    so->getMatrix().getMatrix(m);
    for (unsigned int i = 0; i < so->getNumPoints(); ++i) {
      float x = v[i * 3 + 0];
      float y = v[i * 3 + 1];
      float z = v[i * 3 + 2];
      float w = 1.0f;

      // Transform the points: perform the scaling and then transform the
      // points back again
      float nx = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3] * w;
      float ny = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3] * w;
      float nz = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3] * w;
      float nw = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3] * w;

      x = nx / nw;
      y = ny / nw;
      z = nz / nw;

      if (z_align) z -= min[2];

      // Scale points
      x *= scale_x;
      y *= scale_y;
      z *= scale_z;

      nx = m[0][0] * x + m[1][0] * y + m[2][0] * z + m[3][0] * w;
      ny = m[0][1] * x + m[1][1] * y + m[2][1] * z + m[3][1] * w;
      nz = m[0][2] * x + m[1][2] * y + m[2][2] * z + m[3][2] * w;
      nw = m[0][3] * x + m[1][3] * y + m[2][3] * z + m[3][3] * w;

      x = nx / nw;
      y = ny / nw;
      z = nz / nw;

      v[i * 3 + 0] = x;
      v[i * 3 + 1] = y;
      v[i * 3 + 2] = z;

    }
  }
}


LibModelFile::LibModelFile() : Model(), 
  m_initialised(false)
{
  m_config.sige.connect(SigC::slot(*this, &LibModelFile::varconf_error_callback));
}

LibModelFile::~LibModelFile() {
  assert(m_initialised == false);
}
  
int LibModelFile::init(const std::string &filename) {
  assert(m_initialised == false);

  std::string object;
  if (m_config.readFromFile(filename)) {
    if (m_config.findItem(SECTION_model, KEY_filename)) {
      object = (std::string)m_config.getItem(SECTION_model, KEY_filename);
    } else {
      fprintf(stderr, "[LibModelFile] Error: No md3 filename specified.\n");
      return 1;
    }
  } else {
    fprintf(stderr, "[LibModelFile] Error reading %s as varconf file. Trying as .md3 file.\n",
            filename.c_str());
    object = filename;
  }
  // Initialise transform matrix
  float matrix[4][4];
  for (int j = 0; j < 4; ++j) {
    for (int i = 0; i < 4; ++i) {
      if (i == j) matrix[j][i] = 1.0f;
      else matrix[j][i] = 0.0f;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_rotation)) {
    std::string str=(std::string)m_config.getItem(SECTION_model, KEY_rotation);
    float w,x,y,z;
    sscanf(str.c_str(), "%f;%f;%f;%f", &w, &x, &y, &z);
    WFMath::Quaternion q(w,x,y,z);
    QuatToMatrix(q, matrix);
  }
  if (m_config.findItem(SECTION_model, KEY_scale)) {
    double s = (double)m_config.getItem(SECTION_model, KEY_scale);
    for (int i = 0; i < 4; ++i) matrix[i][i] *= s;
  }

  System::instance()->getFileHandler()->expandString(object);

  // Load md3 file
  if (debug) printf("[LibModelFile] Loading: %s\n", object.c_str());

  libmd3_file *modelFile = libmd3_file_load(object.c_str());
  if (!modelFile) {
    fprintf(stderr, "[LibModelFile] Error loading %s file\n", object.c_str());
    return 1;
  }

  for (int i = 0; i < modelFile->header->mesh_count; ++i) {
    libmd3_unpack_normals(&modelFile->meshes[i]);
  }

  // Get mesh data
  libmd3_mesh *meshp = modelFile->meshes;
  for (int i = 0; i < modelFile->header->mesh_count; ++i, ++meshp) {
    SPtrShutdown<StaticObject> so(new StaticObject());
    so->init();

    // Get Texture data from Mesh
    int texture_id = 0, texture_mask_id = 0;
    if (meshp->mesh_header->skin_count != 0) {
      std::string name = (const char*)(meshp->skins[0].name);
      m_config.clean(name);

      // Check for texture name overrides in vconf file
      // Backwards compatibility.
      if (m_config.findItem(name, KEY_filename)) {
        name = (std::string)m_config.getItem(name, KEY_filename);
      }
      // Check for texture name overrides in vconf file
      // New method
      if (m_config.findItem(name, KEY_texture_map_0)) {
        name = (std::string)m_config.getItem(name, KEY_texture_map_0);
      }
      // Request Texture ID
      texture_id = RenderSystem::getInstance().requestTexture(name);
      texture_mask_id = RenderSystem::getInstance().requestTexture(name, true);

      float m[4];
      if (m_config.findItem(name, KEY_ambient)) {
        std::string str = (std::string)m_config.getItem(name, KEY_ambient);
        sscanf(str.c_str(), "%f;%f;%f;%f", &m[0], &m[1], &m[2], &m[3]);
        so->setAmbient(m); 
      }
      if (m_config.findItem(name, KEY_diffuse)) {
        std::string str = (std::string)m_config.getItem(name, KEY_diffuse);
        sscanf(str.c_str(), "%f;%f;%f;%f", &m[0], &m[1], &m[2], &m[3]);
        so->setDiffuse(m); 
      }
      if (m_config.findItem(name, KEY_specular)) {
        std::string str = (std::string)m_config.getItem(name, KEY_specular);
        sscanf(str.c_str(), "%f;%f;%f;%f", &m[0], &m[1], &m[2], &m[3]);
        so->setSpecular(m); 
      }
      if (m_config.findItem(name, KEY_emission)) {
        std::string str = (std::string)m_config.getItem(name, KEY_emission);
        sscanf(str.c_str(), "%f;%f;%f;%f", &m[0], &m[1], &m[2], &m[3]);
        so->setEmission(m); 
      }
      if (m_config.findItem(name, KEY_shininess)) {
        so->setShininess((double)m_config.getItem(name, KEY_shininess));
      }
      
    }
    // Set the transform
    so->getMatrix().setMatrix(matrix);
    // Set the textures
    so->setTexture(0, texture_id, texture_mask_id);
    so->setNumPoints(meshp->mesh_header->vertex_count);
    so->setNumFaces(meshp->mesh_header->triangle_count);

    // Copy data into array.
    so->createVertexData(meshp->mesh_header->vertex_count * 3);
    float *ptr = so->getVertexDataPtr();
    for (int i = 0; i < meshp->mesh_header->vertex_count * 3; ++i) {
      ptr[i] = default_scale * (float)meshp->vertices[i];
    }  
 
    so->copyTextureData(meshp->texcoords, meshp->mesh_header->vertex_count * 2);
    so->copyNormalData(meshp->normals, meshp->mesh_header->vertex_count * 3);

    so->copyIndices(meshp->triangles, meshp->mesh_header->triangle_count * 3);

    m_static_objects.push_back(so);
  }

  libmd3_file_free(modelFile);

  bool z_align = false;
  if (m_config.findItem(SECTION_model, KEY_z_align)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_z_align)) {
      z_align = true;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_isotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic)) {
      scale_object(m_static_objects, AXIS_ALL, true, z_align);
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_x)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_x)) {
      scale_object(m_static_objects, AXIS_X, true, z_align);
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_y)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_y)) {
      scale_object(m_static_objects, AXIS_Y, true, z_align);
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_z)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_z)) {
      scale_object(m_static_objects, AXIS_Z, true, z_align);
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_anisotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_anisotropic)) {
      scale_object(m_static_objects, AXIS_ALL, false, z_align);
    }
  }

  m_initialised = true;
  return 0;
}

int LibModelFile::shutdown() {
  assert(m_initialised);
  m_initialised = false;

  //  Clean up OpenGL data
  contextDestroyed(true);

  return 0;
}

void LibModelFile::contextCreated() {}

void LibModelFile::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  for (; I != m_static_objects.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void LibModelFile::render(bool select_mode) {
  for (StaticObjectList::const_iterator I = m_static_objects.begin(); I != m_static_objects.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->render(select_mode);
  }
}

void LibModelFile::varconf_error_callback(const char *message) {
  fprintf(stderr, "[LibModelFile] %s\n", message);
}

} /* namespace Sear */
