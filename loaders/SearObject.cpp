// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: SearObject.cpp,v 1.11 2007-05-26 21:09:55 simon Exp $

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

#include "StaticObjectFunctions.h"

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
static const std::string KEY_ignore_minus_z = "ignore_minus_z";
static const std::string KEY_z_align = "z_align";
static const std::string KEY_align_extent = "align_extent";
static const std::string KEY_align_mass = "align_mass";
static const std::string KEY_align_bbox_lc = "align_bbox_lc";
static const std::string KEY_align_bbox_hc = "align_bbox_hc";

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

SearObject::SearObject() : Model(),
  m_initialised(false)
{
  m_config.sige.connect(sigc::mem_fun(this, &SearObject::varconf_error_callback));
}

SearObject::~SearObject() {
  if (m_initialised) shutdown();
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
    fprintf(stderr, "[SearObject] Error reading %s as varconf file. Trying as SearObject file.\n", file_name.c_str());
    object = file_name;
  }

  System::instance()->getFileHandler()->getFilePath(object);

  // Load SearObject file
  if (load(object)) {
    //  Error loading object
    fprintf(stderr, "[SearObject] Error loading SearObject %s\n", object.c_str());
    m_static_objects.clear();
    return 1;
  }


 // Initialise transform matrix
  float matrix[4][4];
  for (int j = 0; j < 4; ++j) {
    for (int i = 0; i < 4; ++i) {
      if (i == j) matrix[j][i] = 1.0f;
      else matrix[j][i] = 0.0f;
    }
  }
  bool do_transform = false;
  if (m_config.findItem(SECTION_model, KEY_rotation)) {
    const std::string &str=(std::string)m_config.getItem(SECTION_model, KEY_rotation);
    float w,x,y,z;
    sscanf(str.c_str(), "%f;%f;%f;%f", &w, &x, &y, &z);
    WFMath::Quaternion q(w,x,y,z);
    QuatToMatrix(q, matrix);
    do_transform = true;
  }
  if (m_config.findItem(SECTION_model, KEY_scale)) {
    double s = (double)m_config.getItem(SECTION_model, KEY_scale);
    for (int i = 0; i < 4; ++i) matrix[i][i] *= s;
    do_transform = true;
  }
  if (do_transform) {
    transform_object(m_static_objects, matrix);
  }

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
  if (m_config.findItem(SECTION_model, KEY_align_mass)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_align_mass)) {
      process_model = true;
      align = ALIGN_CENTRE_MASS;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_align_bbox_hc)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_align_bbox_hc)) {
      process_model = true;
      align = ALIGN_BBOX_HC;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_align_bbox_lc)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_align_bbox_lc)) {
      process_model = true;
      align = ALIGN_BBOX_LC;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_align_extent)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_align_extent)) {
      process_model = true;
      align = ALIGN_CENTRE_EXTENT;
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

int SearObject::shutdown() {
  assert(m_initialised == true);

  // Clean up OpenGL bits
  contextDestroyed(true);

  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    int id, mask_id;
    // Clean up textures
    if (so->getTexture(0, id, mask_id) == 0) {
      RenderSystem::getInstance().releaseTexture(id);
      RenderSystem::getInstance().releaseTexture(mask_id);
    }
  }

  m_static_objects.clear();

  m_initialised = false;
  return 0;
}

void SearObject::contextCreated() {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    so->contextCreated();
  }
}

void SearObject::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void SearObject::render(bool select_mode) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  StaticObjectList::const_iterator Iend = m_static_objects.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
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

    SPtr<StaticObject> so(new StaticObject());
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
      c = som.num_vertices * 2;
      while (c--) swap_bytes_float(*fptr++);
    }

    if (som.num_faces > 0) {
      so->createIndices(som.num_faces * 3);
      fread(so->getIndicesPtr(), sizeof(uint32_t), som.num_faces * 3, fp);
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
