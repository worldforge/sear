// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: SearObject.cpp,v 1.2 2007-01-27 11:38:47 simon Exp $

#include  <stdio.h>

#include <sage/sage.h>
#include <sage/GL.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/System.h"
#include "src/FileHandler.h"
#include "renderers/Graphics.h"
#include "renderers/Render.h"
#include "renderers/RenderSystem.h"
#include "StaticObject.h"

#include "SearObject.h"
#include "SearObjectTypes.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

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
/*
static void swap_bytes_uint16_t(uint16_t &i) {
  union { uint16_t uint; char c[2]; } u;
  u.uint = i;
  char t = u.c[0];
  u.c[0] = u.c[1];
  u.c[1] = t;
}
*/
static void swap_bytes_uint32_t(uint32_t &i) {
  union { uint32_t uint; char c[4]; } u;
  u.uint = i;
  char t = u.c[0];
  u.c[0] = u.c[3];
  u.c[3] = t;
  t = u.c[1];
  u.c[1] = u.c[2];
  u.c[2] = t;
}

static void swap_bytes_float(float &i) {
  union { float f; char c[4]; } u;
  u.f = i;
  char t = u.c[0];
  u.c[0] = u.c[3];
  u.c[3] = t;
  t = u.c[1];
  u.c[1] = u.c[2];
  u.c[2] = t;
}


typedef enum {
  AXIS_ALL = 0,
  AXIS_X,
  AXIS_Y,
  AXIS_Z
} Axis;

static void scale_object(StaticObjectList &objs, Axis axis, bool isotropic, bool z_align, bool ignore_minus_z) {

  float min[3] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
  float max[3] = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

  // Find bounds of object
  for (StaticObjectList::const_iterator I = objs.begin(); I != objs.end(); ++I) {
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

      if (x < min[0]) min[0] = x; 
      if (y < min[1]) min[1] = y; 
      if (z < min[2]) min[2] = z; 

      if (x > max[0]) max[0] = x; 
      if (y > max[1]) max[1] = y; 
      if (z > max[2]) max[2] = z; 
    }
  }

  if (ignore_minus_z) {
    min[2] = 0.0f;
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
      case AXIS_Z: // Scale so Z acis is 1.0
        diff_x = diff_y = diff_z;
        break;
      default:
        printf("Unknown axis, scaling to largest\n");
      case AXIS_ALL: // Scale so largest axis is 1.0
        diff_x = diff_y = diff_z = std::max(std::max(diff_x, diff_y), diff_z);
        break;
    }
  }

  float scale_x = 1.0 / (diff_x);
  float scale_y = 1.0 / (diff_y);
  float scale_z = 1.0 / (diff_z);

  for (StaticObjectList::const_iterator I = objs.begin(); I != objs.end(); ++I) {
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

SearObject::SearObject() : Model(),
  m_initialised(false)
{
  m_config.sige.connect(sigc::mem_fun(this, &SearObject::varconf_error_callback));
}

SearObject::~SearObject() {
  assert(m_initialised == false);
}

int SearObject::init(const std::string &file_name) {
  assert(m_initialised == false);
 
  std::string object;
  if (m_config.readFromFile(file_name)) {
    if (m_config.findItem(SECTION_model, KEY_filename)) {
      object = (std::string)m_config.getItem(SECTION_model, KEY_filename);
    } else {
      fprintf(stderr, "[SearObject] Error: No SearObject filename specified.\n");
      return 1;
    }
  } else {
    fprintf(stderr, "[SearObject] Error reading %s as varconf file. Trying as SwearObject file.\n", file_name.c_str());
    object = file_name;
  }

  System::instance()->getFileHandler()->getFilePath(object);

  // Load SearObject file
  if (debug) printf("[SearObject] Loading: %s\n", object.c_str());

  if (load(object)) {
    //  Error loading object
    fprintf(stderr, "[SearObject] Error loading SearObject %s\n", object.c_str());
    m_static_objects.clear();
    return 1;
  }

  bool z_align = false;
  bool ignore_minus_z = false;

  if (m_config.findItem(SECTION_model, KEY_ignore_minus_z)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_ignore_minus_z)) {
      ignore_minus_z = true;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_z_align)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_z_align)) {
      z_align = true;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_isotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic)) {
      scale_object(m_static_objects, AXIS_ALL, true, z_align, ignore_minus_z);
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_x)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_x)) {
      scale_object(m_static_objects, AXIS_X, true, z_align, ignore_minus_z);
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_y)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_y)) {
      scale_object(m_static_objects, AXIS_Y, true, z_align, ignore_minus_z);
    }
  }
  else if (m_config.findItem(SECTION_model, KEY_scale_isotropic_z)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic_z)) {
      scale_object(m_static_objects, AXIS_Z, true, z_align, ignore_minus_z);
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_anisotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_anisotropic)) {
      scale_object(m_static_objects, AXIS_ALL, false, z_align, ignore_minus_z);
    }
  }


  contextCreated();

  m_initialised = true;

  return 0;
}

int SearObject::shutdown() {
  assert(m_initialised == true);

  // Clean up OpenGL bits
  contextDestroyed(true);

  m_static_objects.clear();

  m_initialised = false;
  return 0;
}

void SearObject::contextCreated() {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextCreated();
  }
}

void SearObject::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void SearObject::render(bool select_mode) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->render(select_mode);
  }
}

void SearObject::varconf_error_callback(const char *message) {
  fprintf(stderr, "[SearObject] %s\n", message);
}


int SearObject::load(const std::string &filename) {
  bool big_endian = false;

  FILE *fp = fopen(filename.c_str(), "rb");
  if (fp == 0) {
    fprintf(stderr, "[SearObject] Error opening %s for reading\n", filename.c_str());
    return 1;
  }

  SearObjectHeader soh;
  if (fread(&soh, sizeof(SearObjectHeader), 1, fp) != 1) {
    // Error reading header
    fprintf(stderr, "[SearObject] Error reading SearObject header in %s\n", filename.c_str());
    fclose(fp);
    return 1;
  }
  
  // Check Magic
  if (strncmp(soh.magic, "SEARSTAT", 8)) {
   fprintf(stderr, "[SearObject] Bad magic - unknown file format.\n");
   fclose(fp);
   return 1;
  }

  // Check Endianess
  // Does this check actually work? Or should we convert into a chars and
  // compare char order instead?
  if (soh.byte_order == 0x00FF) {
    big_endian = true;
    printf("[SearObject] Swapping byte order\n");
    swap_bytes_uint32_t(soh.num_meshes);
  }

  // Check Version
  if (soh.version != 1) {
    fprintf(stderr, "[SearObject] SearObject Version %d is unsupported. Version %d expected.\n", soh.version, 1);
    fclose(fp);
    return 1;
  } 

  SearObjectMesh som;
  int tex_id, tex_mask_id;
  uint32_t *uptr;
  float *fptr;
  int c,x,y;
  for (uint32_t i = 0; i < soh.num_meshes; ++i) {
    if (fread(&som, sizeof(SearObjectMesh), 1, fp) != 1) {
      fprintf(stderr, "[SearObject] Error reading SearObject Mesh in %s\n", filename.c_str());
      fclose(fp);
      return 1;
    }

    if (big_endian) {
      swap_bytes_uint32_t(som.num_vertices);
      swap_bytes_uint32_t(som.num_faces);

      for (x = 0; x < 4; ++x) {
        for (y = 0; y < 4; ++y) {
          swap_bytes_float(som.mesh_transform[x][y]);
          swap_bytes_float(som.texture_transform[x][y]);
        }
      }
      for (x = 0; x < 4; ++x) {
        swap_bytes_float(som.ambient[x]);
        swap_bytes_float(som.diffuse[x]);
        swap_bytes_float(som.specular[x]);
        swap_bytes_float(som.emissive[x]);
      }
      swap_bytes_float(som.shininess);
    }

    SPtrShutdown<StaticObject> so(new StaticObject());
    so->init();
   
    so->setNumPoints(som.num_vertices);
    so->setNumFaces(som.num_faces);

    // Does this use all 256 chars, or only up to 256?
    som.texture_map[255] = '\0'; // Make sure the string is null-terminated
    std::string tex_name(som.texture_map);

    // See if config file has a mapping
    m_config.clean(tex_name);
    if (m_config.findItem(tex_name, KEY_texture_map_0)) {
      tex_name = (std::string)m_config.getItem(tex_name, KEY_texture_map_0);
    }

    // Get texture ids
    tex_id =  RenderSystem::getInstance().requestTexture(tex_name);
    tex_mask_id =  RenderSystem::getInstance().requestTexture(tex_name, true);
    so->setTexture(0, tex_id, tex_mask_id);   

    // Set transform matrices
    so->getMatrix().setMatrix(som.mesh_transform);
    so->getTexMatrix().setMatrix(som.texture_transform);

    // Set Materials
    so->setAmbient(som.ambient);
    so->setDiffuse(som.diffuse);
    so->setSpecular(som.specular);
    so->setEmission(som.emissive);
    so->setShininess(som.shininess);

    // Read in the vertex data array 
    so->createVertexData(som.num_vertices * 3);
    fread(so->getVertexDataPtr(), sizeof(float), som.num_vertices * 3, fp);
    if (big_endian) {
      fptr = so->getVertexDataPtr();
      c = som.num_vertices * 3;
      while (c--) swap_bytes_float(*fptr++);
    }

    so->createNormalData(som.num_vertices * 3);
    fread(so->getNormalDataPtr(), sizeof(float), som.num_vertices * 3, fp);
    if (big_endian) {
      fptr = so->getNormalDataPtr();
      c = som.num_vertices * 3;
      while (c--) swap_bytes_float(*fptr++);
    }

    so->createTextureData(som.num_vertices * 2);
    fread(so->getTextureDataPtr(), sizeof(float), som.num_vertices * 2, fp);
    if (big_endian) {
      fptr = so->getTextureDataPtr();
      c = som.num_vertices * 3;
      while (c--) swap_bytes_float(*fptr++);
    }

    if (som.num_faces > 0) {
      so->createIndices(som.num_faces * 3);
      fread(so->getIndicesPtr(), sizeof(uint32_t), som.num_vertices * 2, fp);
      if (big_endian) {
        uptr = (uint32_t*)so->getIndicesPtr();
        c = som.num_faces * 3;
        while (c--) swap_bytes_uint32_t(*uptr++);
      }
    }

    m_static_objects.push_back(so);
  }  

  fclose(fp);

  return 0;
}



} /* namespace Sear */
