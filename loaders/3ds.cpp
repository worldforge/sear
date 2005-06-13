// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: 3ds.cpp,v 1.46 2005-06-13 15:10:46 simon Exp $

#include <iostream>
#include <list>
#include <map>
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

class RenderObject {
public:
   RenderObject() : 
     vertex_data(NULL),
     normal_data(NULL),
     texture_data(NULL),
     num_points(0),
     texture_id(0),
     texture_mask_id(0),
     vb_vertex_data(0),
     vb_texCoords_data(0),
     vb_normal_data(0)
 {}

    Vertex_3 *vertex_data;
    Normal *normal_data;
    Texel *texture_data;
    unsigned int num_points;
    int texture_id;
    int texture_mask_id;
    std::string material_name;

    GLuint vb_vertex_data;
    GLuint vb_texCoords_data;
    GLuint vb_normal_data;
};


ThreeDS::ThreeDS(Render *render) : Model(render),
  m_initialised(false),
  m_list(0),
  m_list_select(0),
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
  invalidate();

  while (!m_render_objects.empty()) {
    RenderObject *ro = *m_render_objects.begin();
    if (ro) {
      if (ro->vertex_data) delete [] (ro->vertex_data);
      if (ro->texture_data) delete [] (ro->texture_data);
      if (ro->normal_data) delete [] (ro->normal_data);

      delete ro;
    }
    m_render_objects.erase(m_render_objects.begin());
  }
  while (!m_material_map.empty()) {
    Material *m = m_material_map.begin()->second;
    assert(m);
    delete m;
    m_material_map.erase(m_material_map.begin());
  }

  m_initialised = false;
  return 0;
}

void ThreeDS::invalidate() {
  assert(m_render);

  // Clean up display lists
  m_render->freeList(m_list);
  m_list = 0;

  m_render->freeList(m_list_select);
  m_list_select = 0;

  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    for (std::list<RenderObject*>::const_iterator I = m_render_objects.begin();
                                                  I != m_render_objects.end();
                                                  ++I) {
      RenderObject *ro = *I;
      if (ro) {
        if (glIsBufferARB(ro->vb_vertex_data)) {
          glDeleteBuffersARB(1, &ro->vb_vertex_data);
          ro->vb_vertex_data = 0;
        }
        if (glIsBufferARB(ro->vb_texCoords_data)) {
          glDeleteBuffersARB(1, &ro->vb_texCoords_data);
          ro->vb_texCoords_data = 0;
        }
        if (glIsBufferARB(ro->vb_normal_data)) {
          glDeleteBuffersARB(1, &ro->vb_normal_data);
          ro->vb_normal_data = 0;
        }
      }
    }
  }
}

void ThreeDS::render(bool select_mode) {
  assert(m_render);

  std::string current_material = "sear:blank";
  m_config.clean(current_material);

  m_render->scaleObject(m_height);
  bool end_list = false;
  if (select_mode) {
    if (m_list_select) {
      m_render->playList(m_list_select);
      return;
    } else {
      if (!sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
        m_list_select = m_render->getNewList();
        m_render->beginRecordList(m_list_select);
        end_list = true;
      }
    }
  } else  {
    if (m_list) {
      m_render->playList(m_list);
      return;
    } else {
      if (!sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
        m_list = m_render->getNewList();
        m_render->beginRecordList(m_list);
        end_list = true;
      }
    }
  }

  for (std::list<RenderObject*>::const_iterator I = m_render_objects.begin(); I != m_render_objects.end(); ++I) {
    RenderObject *ro = *I;
    assert(ro);

    if (ro->material_name != current_material) {
      MaterialMap::const_iterator I = m_material_map.find(ro->material_name);
      if (I != m_material_map.end()) {
        Material *m = I->second;
        assert(m);
        m_render->setMaterial(m->ambient, m->diffuse, m->specular, m->shininess, NULL);
      }
      current_material = ro->material_name;
    }
    if (ro->texture_data) {
      if (select_mode) {
        RenderSystem::getInstance().switchTexture(ro->texture_mask_id);
      } else {
        RenderSystem::getInstance().switchTexture(ro->texture_id);
      }
    } else {
      RenderSystem::getInstance().switchTexture(0);
    }
    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
      // Generate VBO's if required
      if (!glIsBufferARB(ro->vb_vertex_data)) {
        glGenBuffersARB(1, &ro->vb_vertex_data);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, ro->vb_vertex_data);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, ro->num_points * 3 * sizeof(float), ro->vertex_data, GL_STATIC_DRAW_ARB);
        if (ro->normal_data != NULL) {
          glGenBuffersARB(1, &ro->vb_normal_data);
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, ro->vb_normal_data);
          glBufferDataARB(GL_ARRAY_BUFFER_ARB, ro->num_points * 3 * sizeof(float), ro->normal_data, GL_STATIC_DRAW_ARB);
        }
        if (ro->texture_data != NULL) {
          glGenBuffersARB(1, &ro->vb_texCoords_data);
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, ro->vb_texCoords_data);
          glBufferDataARB(GL_ARRAY_BUFFER_ARB, ro->num_points * 2 * sizeof(float), ro->texture_data, GL_STATIC_DRAW_ARB);
        }
      }

      glBindBufferARB(GL_ARRAY_BUFFER_ARB, ro->vb_vertex_data);
      glVertexPointer(3, GL_FLOAT, 0, NULL);

      if (ro->normal_data != NULL) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, ro->vb_normal_data);
        glNormalPointer(GL_FLOAT, 0, NULL);
      }

      if (ro->texture_data != NULL) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, ro->vb_texCoords_data);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
      }
      // Draw object
      glDrawArrays(GL_TRIANGLES, 0, ro->num_points);
      // Reset states
      if (ro->normal_data != NULL) {
        glDisableClientState(GL_NORMAL_ARRAY);
      }
      if (ro->texture_data != NULL) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      }
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    } else {
      m_render->renderArrays(Graphics::RES_TRIANGLES, 0, ro->num_points, ro->vertex_data, ro->texture_data, ro->normal_data, false);
    }
  }
  if (end_list) {
    m_render->endRecordList();
  }
}

void ThreeDS::render_node(Lib3dsNode *node, Lib3dsFile *file) {
  Lib3dsNode *p;
  Lib3dsObjectData *d;
  d=&node->data.object;
  for (p=node->childs; p!=0; p=p->next) {
    render_node(p, file);
  }
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

  RenderObject *ro = NULL;//new RenderObject();
//  m_render_objects.push_back(ro);
//  ro->num_points = 3 * mesh->faces;
//  ro->vertex_data = new Vertex_3[ro->num_points];
//  ro->normal_data = new Normal[ro->num_points];
//  ro->texture_data = (mesh->texels) ? (new Texel[ro->num_points]) : (NULL);
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
    int texture_id = -1;
    int texture_mask_id = -1;
    // If a material is set get texture map names.
    if (mat && mat->texture1_map.name[0]) {

      if (m_config.findItem(material_name,"texture_map_0")) {
        std::string name = (std::string)m_config.getItem(material_name,"texture_map_0");
        texture_id = RenderSystem::getInstance().requestTexture(name);
        texture_mask_id = RenderSystem::getInstance().requestTexture(name, true);
      } else  {
        texture_id = RenderSystem::getInstance().requestTexture(
                                                        mat->texture1_map.name);
        texture_mask_id = RenderSystem::getInstance().requestTexture(
                                                  mat->texture1_map.name, true);
      }
    } else {
      texture_id = 0;
      texture_mask_id = 0;
    }
    // Create new render object for change in texture
    if (texture_id != current_texture) {
      current_texture = texture_id;
      // Set correct num of points in old render object
      if (ro) ro->num_points = v_counter;

      // Reset counters
      v_counter = n_counter = t_counter = 0;
      // Create a new render object and create data structures        
      ro = new RenderObject();
      ro->texture_id = texture_id;
      ro->texture_mask_id = texture_mask_id;
      ro->num_points = 3 * mesh->faces;
      ro->vertex_data = new Vertex_3[ro->num_points];
      ro->normal_data = new Normal[ro->num_points];
      ro->texture_data = (mesh->texels) ? (new Texel[ro->num_points]) : (NULL);
      ro->material_name = material_name;

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
      lib3ds_vector_transform((float*)&ro->vertex_data[v_counter++], mesh->matrix, out);
  
       /* It is very likely the normals have been completely messed up by these transformations */
      lib3ds_vector_transform(out, M,  normalL[3 * p + i]);
      if (d) {
        out[0] -= d->pivot[0];
        out[1] -= d->pivot[1];
        out[2] -= d->pivot[2];
      }
      lib3ds_vector_transform((float*)&ro->normal_data[n_counter], mesh->matrix, out);
      ++n_counter;
  
      if (mesh->texels) {
        ro->texture_data[t_counter].s = mesh->texelL[f->points[i]][0];
        ro->texture_data[t_counter++].t = mesh->texelL[f->points[i]][1];
      }
    }
  }
 
  ro->num_points = v_counter;
  free(normalL);
}

void ThreeDS::varconf_error_callback(const char *message) {
  fprintf(stderr, "3DS: %s\n", message);
}

} /* namespace Sear */
