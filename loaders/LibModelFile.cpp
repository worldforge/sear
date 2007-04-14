// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2007 Simon Goodall

// $Id: LibModelFile.cpp,v 1.32 2007-04-14 13:46:42 simon Exp $

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
#include "StaticObjectFunctions.h"

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
static const std::string KEY_ignore_minus_z = "ignore_minus_z";

static const std::string KEY_texture_map_0 = "texture_map_0";
static const std::string KEY_ambient = "ambient";
static const std::string KEY_diffuse = "diffuse";
static const std::string KEY_specular = "specular";
static const std::string KEY_emission = "emission";
static const std::string KEY_shininess = "shininess";

LibModelFile::LibModelFile() : Model(), 
  m_initialised(false)
{
  m_config.sige.connect(sigc::mem_fun(this, &LibModelFile::varconf_error_callback));
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
    const std::string &str=(std::string)m_config.getItem(SECTION_model, KEY_rotation);
    float w,x,y,z;
    sscanf(str.c_str(), "%f;%f;%f;%f", &w, &x, &y, &z);
    WFMath::Quaternion q(w,x,y,z);
    QuatToMatrix(q, matrix);
  }
  if (m_config.findItem(SECTION_model, KEY_scale)) {
    double s = (double)m_config.getItem(SECTION_model, KEY_scale);
    for (int i = 0; i < 4; ++i) matrix[i][i] *= s;
  }

  System::instance()->getFileHandler()->getFilePath(object);

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
        const std::string &str = (std::string)m_config.getItem(name, KEY_ambient);
        sscanf(str.c_str(), "%f;%f;%f;%f", &m[0], &m[1], &m[2], &m[3]);
        so->setAmbient(m); 
      }
      if (m_config.findItem(name, KEY_diffuse)) {
        const std::string &str = (std::string)m_config.getItem(name, KEY_diffuse);
        sscanf(str.c_str(), "%f;%f;%f;%f", &m[0], &m[1], &m[2], &m[3]);
        so->setDiffuse(m); 
      }
      if (m_config.findItem(name, KEY_specular)) {
        const std::string &str = (std::string)m_config.getItem(name, KEY_specular);
        sscanf(str.c_str(), "%f;%f;%f;%f", &m[0], &m[1], &m[2], &m[3]);
        so->setSpecular(m); 
      }
      if (m_config.findItem(name, KEY_emission)) {
        const std::string &str = (std::string)m_config.getItem(name, KEY_emission);
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


  bool ignore_minus_z = false;

  Scaling scale = SCALE_NONE;
  Alignment align = ALIGN_NONE;
  bool process_model = false;

  if (m_config.findItem(SECTION_model, KEY_ignore_minus_z)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_ignore_minus_z)) {
      process_model = true;
      ignore_minus_z = true;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_z_align)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_z_align)) {
      process_model = true;
      align = ALIGN_Z;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_isotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic)) {
      process_model = true;
      scale = SCALE_ISOTROPIC;
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_x)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_x)) {
      process_model = true;
      scale = SCALE_ISOTROPIC_Z;
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_y)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_y)) {
      process_model = true;
      scale = SCALE_ISOTROPIC_Y;
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_z)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_z)) {
      process_model = true;
      scale = SCALE_ISOTROPIC_Z;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_anisotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_anisotropic)) {
      process_model = true;
      scale = SCALE_ANISOTROPIC;
    }
  }
  
  if (process_model == true) {
    scale_object(m_static_objects, scale, align, ignore_minus_z);
  }


  contextCreated();

  m_initialised = true;
  return 0;
}

int LibModelFile::shutdown() {
  assert(m_initialised);
  m_initialised = false;

  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    int id, mask_id;
    // Clean up textures
    if (so->getTexture(0, id, mask_id) == 0) {
      RenderSystem::getInstance().releaseTexture(id);
      RenderSystem::getInstance().releaseTexture(mask_id);
    }
  }

  //  Clean up OpenGL data
  contextDestroyed(true);

  return 0;
}

void LibModelFile::contextCreated() {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextCreated();
  }
}

void LibModelFile::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void LibModelFile::render(bool select_mode) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->render(select_mode);
  }
}

void LibModelFile::varconf_error_callback(const char *message) {
  fprintf(stderr, "[LibModelFile] %s\n", message);
}

} /* namespace Sear */
