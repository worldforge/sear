// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: 3ds.cpp,v 1.51 2006-02-07 17:55:39 simon Exp $

/** TODO
 * - Currently each render object is created to store all the data in the mesh,
 *   however, if the material changes, a new object is created (of the same 
 *   size) but with the new points. Perhaps assume that 1 mesh == 1 material?
 */

#include <iostream>
#include <list>
#include <map>

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

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

ThreeDS::ThreeDS(Render *render) : Model(render),
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
    if (m_config.findItem("model", "filename")) {
      object = (std::string)m_config.getItem("model", "filename");
    } else {
      fprintf(stderr, "Error: No 3ds filename specified.\n");
      return 1;
    }
  } else {
    fprintf(stderr, "Error reading %s as varconf file. Trying as .3ds file.\n", file_name.c_str());
    object = file_name;
  }

  System::instance()->getFileHandler()->expandString(object);

  // Load 3ds file
  if (debug) printf("3ds: Loading: %s\n", object.c_str());

  Lib3dsFile *model = lib3ds_file_load(object.c_str());

  if (!model) {
    fprintf(stderr, "3ds: Unable to load %s\n", object.c_str());
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
    if (debug) printf("3ds: Rendering Meshes direct\n");
    render_file(model);
  } else {
    for (Lib3dsNode *n = model->nodes; n != NULL; n = n->next) {
      render_node(n, model);
    }
  }

  lib3ds_file_free(model);

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
  assert(m_render);

  StaticObjectList::const_iterator I = m_render_objects.begin();
  for (; I != m_render_objects.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }
}

void ThreeDS::render(bool select_mode) {
  assert(m_render);

  m_render->scaleObject(m_height);

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

  SPtrShutdown<StaticObject> ro;// = NULL;
  int current_texture = -2;
  std::string material_name = "sear:noname";

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
        m->ambient[0] = 0.0f;
        m->ambient[1] = 0.0f;
        m->ambient[2] = 0.0f;
        m->ambient[3] = 1.0f;

        m->diffuse[0] = mat->diffuse[0];
        m->diffuse[1] = mat->diffuse[1];
        m->diffuse[2] = mat->diffuse[2];
        m->diffuse[3] = mat->diffuse[3];
        m->specular[0] = mat->specular[0];
        m->specular[1] = mat->specular[1];
        m->specular[2] = mat->specular[2];
        m->specular[3] = mat->specular[3];
        m->shininess = pow(2, 10.0*mat->shininess);
        // Clamp shininess
        if (m->shininess>128.0f) m->shininess = 128.0f;
        
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
      if (m_config.findItem(material_name,"texture_map_0")) {
        std::string name = (std::string)m_config.getItem(material_name,
                                                         "texture_map_0");
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
      ro->setNumPoints(3 * mesh->faces)
;
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
  fprintf(stderr, "3DS: %s\n", message);
}

} /* namespace Sear */
