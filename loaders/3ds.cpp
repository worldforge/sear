// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

// $Id: 3ds.cpp,v 1.73 2007-05-07 10:31:56 simon Exp $

/** TODO
 * - Make Material map only available within loader routines, not as a member
 *   variable.
 * - Move matrix transforms into StaticObject.
 * - There appears to be a hierarchy of meshes that all need the matrix 
 *   combined before storing in StaticObject
 * - We could possibly re-write the 3ds stuff to allow animations?
 *
 * - Currently each render object is created to store all the data in the mesh,
 *   however, if the material changes, a new object is created (of the same 
 *   size) but with the new points. Perhaps assume that 1 mesh == 1 material?
 */

#include <iostream>
#include <list>
#include <map>
#include <algorithm>

#include <sigc++/object_slot.h>

#include <sage/sage.h>
#include <sage/GL.h>

#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/System.h"
#include "src/FileHandler.h"
#include "renderers/Graphics.h"
//#include "renderers/Render.h"
#include "renderers/RenderSystem.h"
#include "StaticObject.h"

#include "3ds.h"

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
static const std::string KEY_z_align = "z_align";
static const std::string KEY_ignore_minus_z = "ignore_minus_z";

static const std::string KEY_texture_map_0 = "texture_map_0";
static const std::string KEY_ambient = "ambient";
static const std::string KEY_diffuse = "diffuse";
static const std::string KEY_specular = "specular";
static const std::string KEY_emission = "emission";
static const std::string KEY_shininess = "shininess";

ThreeDS::ThreeDS() : Model(),
  m_initialised(false)
{
  m_config.sige.connect(sigc::mem_fun(this, &ThreeDS::varconf_error_callback));
}

ThreeDS::~ThreeDS() {
  if (m_initialised) shutdown();
}

int ThreeDS::init(const std::string &file_name) {
  assert(m_initialised == false);
 
  std::string object;
  if (m_config.readFromFile(file_name)) {
    if (m_config.findItem(SECTION_model, KEY_filename)) {
      object = (std::string)m_config.getItem(SECTION_model, KEY_filename);
    } else {
      fprintf(stderr, "[3ds] Error: No 3ds filename specified.\n");
      return 1;
    }
  } else {
    fprintf(stderr, "[3ds] Error reading %s as varconf file. Trying as .3ds file.\n", file_name.c_str());
    object = file_name;
  }

  System::instance()->getFileHandler()->getFilePath(object);

  // Load 3ds file
  if (debug) printf("[3ds] Loading: %s\n", object.c_str());

  Lib3dsFile *model = lib3ds_file_load(object.c_str());

  if (!model) {
    fprintf(stderr, "[3ds] Unable to load %s\n", object.c_str());
    return 1;
  }

  // Create default material
  Material *m = new Material;
  m->ambient[0] = 1.0f;
  m->ambient[1] = 1.0f;
  m->ambient[2] = 1.0f;
  m->ambient[3] = 1.0f;

  m->diffuse[0] = 0.0f;
  m->diffuse[1] = 0.0f;
  m->diffuse[2] = 0.0f;
  m->diffuse[3] = 0.0f;

  m->specular[0] = 0.0f;
  m->specular[1] = 0.0f;
  m->specular[2] = 0.0f;
  m->specular[3] = 0.0f;

  m->shininess = 0.0f;
  std::string mat_name = "sear:default";
  m_config.clean(mat_name);
  m_material_map[mat_name] = m;

  // Calculate initial positions
  lib3ds_file_eval(model,1);
  if (model->nodes == NULL) {
    render_file(model);
  } else {
    for (Lib3dsNode *n = model->nodes; n != NULL; n = n->next) {
      render_node(n, model);
    }
  }

  lib3ds_file_free(model);
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
    scale_object(m_render_objects, scale, align, ignore_minus_z);
  }

  contextCreated();

  m_initialised = true;
  return 0;
}

int ThreeDS::shutdown() {
  assert(m_initialised == true);

  // Clean up OpenGL bits
  contextDestroyed(true);

  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
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

  m_render_objects.clear();

  while (!m_material_map.empty()) {
    Material *m = m_material_map.begin()->second;
    assert(m);
    delete m;
    m_material_map.erase(m_material_map.begin());
  }

  m_initialised = false;
  return 0;
}

void ThreeDS::contextCreated() {
  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    so->contextCreated();
  }
}

void ThreeDS::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void ThreeDS::render(bool select_mode) {
  StaticObjectList::const_iterator I = m_render_objects.begin();
  StaticObjectList::const_iterator Iend = m_render_objects.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    so->render(select_mode);
  }
}

void ThreeDS::render_node(Lib3dsNode *node, Lib3dsFile *file) {
  assert(node);
  assert(file);
  for (Lib3dsNode *p = node->childs; p != 0; p = p->next) {
    render_node(p, file);
  }
  Lib3dsObjectData *d = &node->data.object;
  if (node->type==LIB3DS_OBJECT_NODE) {
    if (strcmp(node->name,"$$$DUMMY")==0) return;
    if (!node->user.d) {
      Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
      render_mesh(mesh, file, d);
    }
  }
}

void ThreeDS::render_file(Lib3dsFile *file) {
  Lib3dsMesh *mesh;
  for (mesh=file->meshes; mesh!=0; mesh=mesh->next) {
    render_mesh(mesh, file, NULL);
  }
}

void ThreeDS::render_mesh(Lib3dsMesh *mesh, Lib3dsFile *file, Lib3dsObjectData *d) {
  unsigned p;
  Lib3dsVector *normalL=(Lib3dsVector*)malloc(3*sizeof(Lib3dsVector)*mesh->faces);
    
  Lib3dsMatrix M;
  lib3ds_matrix_copy(M, mesh->matrix);
  lib3ds_matrix_inv(M);
  lib3ds_mesh_calculate_normals(mesh, normalL);
  unsigned int v_counter = 0;
  unsigned int n_counter = 0;
  unsigned int t_counter = 0;

  SPtr<StaticObject> ro;
  std::string material_name = "sear:noname";
  std::string current_material_name = "sear:bogus_name";

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

  for (p=0; p<mesh->faces; ++p) {
    Lib3dsFace *f=&mesh->faceL[p];
    Lib3dsMaterial *mat = NULL;
    if (f->material[0]) {
      mat = lib3ds_file_material_by_name(file, f->material);
    }
    if (mat) {
      material_name = std::string(f->material);
      m_config.clean(material_name);

      MaterialMap::const_iterator I = m_material_map.find(material_name);
      // If material obj does not exist, create it
      if (I == m_material_map.end()) {
        Material *m = new Material;
        if (m_config.findItem(material_name, KEY_ambient)) {
          const std::string &str = (std::string)m_config.getItem(material_name, KEY_ambient);
          sscanf(str.c_str(), "%f;%f;%f;%f", &m->ambient[0], &m->ambient[1], &m->ambient[2], &m->ambient[3]);
        } else {
          m->ambient[0] = 0.0f;
          m->ambient[1] = 0.0f;
          m->ambient[2] = 0.0f;
          m->ambient[3] = 1.0f;
        }

        if (m_config.findItem(material_name, KEY_diffuse)) {
          const std::string &str = (std::string)m_config.getItem(material_name, KEY_diffuse);
          sscanf(str.c_str(), "%f;%f;%f;%f", &m->diffuse[0], &m->diffuse[1], &m->diffuse[2], &m->diffuse[3]);
        } else {
          m->diffuse[0] = mat->diffuse[0];
          m->diffuse[1] = mat->diffuse[1];
          m->diffuse[2] = mat->diffuse[2];
          m->diffuse[3] = mat->diffuse[3];
        }

        if (m_config.findItem(material_name, KEY_specular)) {
          std::string str = (std::string)m_config.getItem(material_name, KEY_specular);
          sscanf(str.c_str(), "%f;%f;%f;%f", &m->specular[0], &m->specular[1], &m->specular[2], &m->specular[3]);
        } else {
          m->specular[0] = mat->specular[0];
          m->specular[1] = mat->specular[1];
          m->specular[2] = mat->specular[2];
          m->specular[3] = mat->specular[3];
        }

        if (m_config.findItem(material_name, KEY_shininess)) {
          m->shininess = (double)m_config.getItem(material_name, KEY_shininess);
        } else {
          m->shininess = pow(2, 10.0*mat->shininess);
          // Clamp shininess
          if (m->shininess>128.0f) m->shininess = 128.0f;
        }
        
        // Store in material map for later use
        m_material_map[material_name] = m;
      }
    } else {
      // No material? Use default
      material_name = "sear:default";
      m_config.clean(material_name);
    }

    Material *cm = m_material_map[material_name];
    assert(cm);

    int texture_id = 0;
    int texture_mask_id = 0;
    // If a material is set get texture map names.
    Matrix tex_matrix;
    tex_matrix.identity();
    if (mat) {
      float s[4];
      s[0] = mat->texture1_map.scale[0];
      s[1] = mat->texture1_map.scale[1];
      s[2] = s[3] = 1.0f;
      // Is this the correct rotatiom?
      // Or is our combination correct?
      // Need to check matrix
      tex_matrix.rotateZ(deg_to_rad(mat->texture1_map.rotation));
      tex_matrix.scalev(s);
      tex_matrix.translate(mat->texture1_map.offset[0], mat->texture1_map.offset[1], 0.0f);

    }

    if (current_material_name != material_name) {
      if (mesh->texels) {
        if (m_config.findItem(material_name,KEY_texture_map_0)) {
          const std::string &name = (std::string)m_config.getItem(material_name,
                                                         KEY_texture_map_0);
          texture_id = RenderSystem::getInstance().requestTexture(name);
          texture_mask_id = RenderSystem::getInstance().requestTexture(name, true);
          assert(texture_id != 0);
        } else if ( mat->texture1_map.name[0]) {
          texture_id = RenderSystem::getInstance().requestTexture(
                                                          mat->texture1_map.name);
          texture_mask_id = RenderSystem::getInstance().requestTexture(
                                                  mat->texture1_map.name, true);
          assert(texture_id != 0);
        } else { 
          // Do nothing, use default vals
        }
      }
      // Request default texture to keep the reference counting happy.
      if (mesh->texels && texture_id == 0) {
        texture_id = RenderSystem::getInstance().requestTexture("default_texture");
        texture_mask_id = RenderSystem::getInstance().requestTexture("default_texture", true);
      }
      current_material_name = material_name;
      // Create new render object for change in texture
      // Set correct num of points in old render object
      if (ro) ro->setNumPoints(v_counter);

      // Reset counters
      v_counter = n_counter = t_counter = 0;
      // Create a new render object and create data structures
      ro = SPtr<StaticObject>(new StaticObject());
      ro->init();
      ro->setNumPoints(3 * mesh->faces);

      ro->getMatrix().setMatrix(matrix);
      float t[4][4];
      tex_matrix.getMatrix(t);
      ro->getTexMatrix().setMatrix(t);

      ro->createVertexData(ro->getNumPoints() * 3);
      ro->createNormalData(ro->getNumPoints() * 3);
      if (mesh->texels) {
        ro->createTextureData(ro->getNumPoints() * 2);
        ro->setTexture(0, texture_id, texture_mask_id);
      }

      ro->setAmbient(cm->ambient);
      ro->setDiffuse(cm->diffuse);
      ro->setSpecular(cm->specular);
      ro->setEmission(0.0f, 0.0f,0.0f,0.0f);
      ro->setShininess(cm->shininess);

      m_render_objects.push_back(ro);
    }

    int i;
    for (i=0; i<3; ++i) {
      float out[3];
      lib3ds_vector_transform(out, M, mesh->pointL[f->points[i]].pos);
      if (d) {
        out[0] -= d->pivot[0];
        out[1] -= d->pivot[1];
        out[2] -= d->pivot[2];
      }
      lib3ds_vector_transform(&ro->getVertexDataPtr()[v_counter * 3], mesh->matrix, out);
       ++v_counter;
       /* It is very likely the normals have been completely messed up by these transformations */
      lib3ds_vector_transform(out, M,  normalL[3 * p + i]);
      if (d) {
        out[0] -= d->pivot[0];
        out[1] -= d->pivot[1];
        out[2] -= d->pivot[2];
      }
      lib3ds_vector_transform(&ro->getNormalDataPtr()[n_counter * 3], mesh->matrix, out);
      ++n_counter;
  
      if (mesh->texels) {
        ro->getTextureDataPtr()[t_counter++] = mesh->texelL[f->points[i]][0];
        ro->getTextureDataPtr()[t_counter++] = mesh->texelL[f->points[i]][1];
      }
    }
  }
 
  ro->setNumPoints(v_counter);
  free(normalL);
}

void ThreeDS::varconf_error_callback(const char *message) {
  fprintf(stderr, "[3DS] %s\n", message);
}

} /* namespace Sear */
