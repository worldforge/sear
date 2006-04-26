// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: 3ds.cpp,v 1.57 2006-04-26 13:58:47 simon Exp $

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
#include "renderers/Render.h"
#include "renderers/RenderSystem.h"
#include "StaticObject.h"

#include "3ds.h"

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
static const std::string KEY_scale_anisotropic = "scale_anisotropic";
static const std::string KEY_z_align = "z_align";

static const std::string KEY_texture_map_0 = "texture_map_0";
static const std::string KEY_ambient = "ambient";
static const std::string KEY_diffuse = "diffuse";
static const std::string KEY_specular = "specular";
static const std::string KEY_emission = "emission";
static const std::string KEY_shininess = "shininess";

static void scale_object(ThreeDS::StaticObjectList &objs, bool isotropic, bool z_align) {
  float min[3], max[3];
  bool firstPoint = true;
  // Find bounds of object
  for (ThreeDS::StaticObjectList::const_iterator I = objs.begin(); I != objs.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    
    float m[4][4];
    so->getMatrix(m);
    float *v = so->getVertexDataPtr();
    for (int i = 0; i < so->getNumPoints(); ++i) {
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
    float f = std::max(diff_x, diff_y);
    diff_x = diff_y = diff_z = std::max(f, diff_z);
  }

  float scale_x = 1.0 / (diff_x);
  float scale_y = 1.0 / (diff_y);
  float scale_z = 1.0 / (diff_z);

  for (ThreeDS::StaticObjectList::const_iterator I = objs.begin(); I != objs.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    
    float *v = so->getVertexDataPtr();
    float m[4][4];
    so->getMatrix(m);
    for (int i = 0; i < so->getNumPoints(); ++i) {
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

ThreeDS::ThreeDS() : Model(),
  m_initialised(false),
  m_height(1.0f)
{
  m_config.sige.connect(SigC::slot(*this, &ThreeDS::varconf_error_callback));
}

ThreeDS::~ThreeDS() {
  assert(m_initialised == false);
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

  System::instance()->getFileHandler()->expandString(object);

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
    if (debug) printf("[3ds] Rendering Meshes direct\n");
    render_file(model);
  } else {
    for (Lib3dsNode *n = model->nodes; n != NULL; n = n->next) {
      render_node(n, model);
    }
  }

  lib3ds_file_free(model);
  bool z_align = false;
  if (m_config.findItem(SECTION_model, KEY_z_align)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_z_align)) {
      z_align = true;
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_isotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_isotropic)) {
      scale_object(m_render_objects, true, z_align);
    }
  }
  if (m_config.findItem(SECTION_model, KEY_scale_anisotropic)) {
    if ((bool)m_config.getItem(SECTION_model, KEY_scale_anisotropic)) {
      scale_object(m_render_objects, false, z_align);
    }
  }

  m_initialised = true;
  return 0;
}

int ThreeDS::shutdown() {
  assert(m_initialised == true);

  // Clean up OpenGL bits
  contextDestroyed(true);

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

void ThreeDS::contextCreated() {}

void ThreeDS::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_render_objects.begin();
  for (; I != m_render_objects.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void ThreeDS::render(bool select_mode) {
  // TODO Combine this scale into the matrix transform
  Render *render = RenderSystem::getInstance().getRenderer();
  render->scaleObject(m_height);

  for (StaticObjectList::const_iterator I = m_render_objects.begin(); I != m_render_objects.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
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

  SPtrShutdown<StaticObject> ro;
  int current_texture = -2;
  std::string material_name = "sear:noname";

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
          std::string str = (std::string)m_config.getItem(material_name, KEY_ambient);
          sscanf(str.c_str(), "%f;%f;%f;%f", &m->ambient[0], &m->ambient[1], &m->ambient[2], &m->ambient[3]);
        } else {
          m->ambient[0] = 0.0f;
          m->ambient[1] = 0.0f;
          m->ambient[2] = 0.0f;
          m->ambient[3] = 1.0f;
        }

        if (m_config.findItem(material_name, KEY_diffuse)) {
          std::string str = (std::string)m_config.getItem(material_name, KEY_diffuse);
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
    if (mat) {
      if (m_config.findItem(material_name,KEY_texture_map_0)) {
        std::string name = (std::string)m_config.getItem(material_name,
                                                         KEY_texture_map_0);
        texture_id = RenderSystem::getInstance().requestTexture(name);
        texture_mask_id = RenderSystem::getInstance().requestTexture(name, true);
      } else if ( mat->texture1_map.name[0]) {
        texture_id = RenderSystem::getInstance().requestTexture(
                                                        mat->texture1_map.name);
        texture_mask_id = RenderSystem::getInstance().requestTexture(
                                                  mat->texture1_map.name, true);
      } else {
        // Do nothing, use default vals
      }
    }
    // Create new render object for change in texture
    if (texture_id != current_texture) {
      current_texture = texture_id;
      // Set correct num of points in old render object
      if (ro) ro->setNumPoints(v_counter);

      // Reset counters
      v_counter = n_counter = t_counter = 0;
      // Create a new render object and create data structures
      ro = SPtrShutdown<StaticObject>(new StaticObject());
      ro->init();
      ro->setNumPoints(3 * mesh->faces);

      ro->setMatrix(matrix);

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
