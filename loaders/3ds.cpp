// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2002 Simon Goodall

#include <list>
#include <map>
#include <GL/gl.h>

#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"


#include "3ds.h"



namespace Sear {

ThreeDS::ThreeDS() : 
  _model (NULL),
  _scale(1.0f)
{

}

ThreeDS::~ThreeDS() {

}

bool ThreeDS::init(const std::string &file_name, float scale, float offset[3]) {
  if (scale) _scale = scale;
  _offset[0] = offset[0];
  _offset[1] = offset[1];
  _offset[2] = offset[2];
  // Load 3ds file
  Log::writeLog(std::string("Loading: ") + file_name, Log::LOG_DEFAULT);
  _model = lib3ds_file_load(file_name.c_str());
  if (!_model) {
    Log::writeLog(std::string("Unable to load ") + file_name, Log::LOG_ERROR);
    return false;
  }
  // Calculate initial positions
  lib3ds_file_eval(_model,1);
  Lib3dsNode *p = NULL;
  for (p=_model->nodes; p!=0; p=p->next) {
    render_node(p, _model);
  }
  lib3ds_file_free(_model);
  _model = NULL;
  return true;
}

void ThreeDS::shutdown() {
}

void ThreeDS::render(bool select_mode) {
  static Render *rend = System::instance()->getGraphics()->getRender();
  int current_texture = 0;
  std::string current_material = "";
  rend->translateObject(_offset[0], _offset[1], _offset[2]);
  if (select_mode) {
    for (std::list<RenderObject*>::const_iterator I = render_objects.begin(); I != render_objects.end(); I++) {
      RenderObject *ro = *I;
      if (ro) {
        rend->renderArrays(Graphics::RES_TRIANGLES, 0, ro->num_points, ro->vertex_data, NULL, ro->normal_data);
      }
    }
  } else {
    for (std::list<RenderObject*>::const_iterator I = render_objects.begin(); I != render_objects.end(); I++) {
      RenderObject *ro = *I;
      if (std::string(ro->material_name) != current_material) {
        Material *m = material_map[ro->material_name];
        if (m) {
          rend->setMaterial(m->ambient, m->diffuse, m->specular, m->shininess, NULL);
          current_material = ro->material_name;
        }	    
      }
      if (ro) {
        if (current_texture != ro->texture_id) {
          if (ro->texture_data) rend->switchTexture(ro->texture_id);
          current_texture = ro->texture_id;
        }
        rend->renderArrays(Graphics::RES_TRIANGLES, 0, ro->num_points, ro->vertex_data, ro->texture_data, ro->normal_data);
      }
    }
  }
}

void ThreeDS::render_node(Lib3dsNode *node, Lib3dsFile *file) {
  static Render *rend = System::instance()->getGraphics()->getRender();
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
      if (!mesh)  return; 
      unsigned p;
      Lib3dsVector *normalL=(Lib3dsVector*)malloc(3*sizeof(Lib3dsVector)*mesh->faces);
      
      Lib3dsMatrix M;
      lib3ds_matrix_copy(M, mesh->matrix);
      lib3ds_matrix_inv(M);
      lib3ds_mesh_calculate_normals(mesh, normalL);
      unsigned int v_counter = 0;
      unsigned int n_counter = 0;
      unsigned int t_counter = 0;
      RenderObject *ro = (RenderObject*)malloc(sizeof(RenderObject));
      memset(ro, 0, sizeof(RenderObject));
      render_objects.push_back(ro);
      ro->num_points = 3 * mesh->faces;
      ro->vertex_data = (float*)malloc(3 * ro->num_points * sizeof(float));
      ro->normal_data = (float*)malloc(3 * ro->num_points * sizeof(float));
      ro->texture_data = (mesh->texels) ? (float*)malloc(2 * ro->num_points * sizeof(float)) : (NULL);
      int current_texture = 0;
      for (p=0; p<mesh->faces; ++p) {
        Lib3dsFace *f=&mesh->faceL[p];
        Lib3dsMaterial *mat=0;
        if (f->material[0]) {
          mat=lib3ds_file_material_by_name(file, f->material);
	  ro->material_name = std::string(f->material).c_str();
        }
        if (mat) {
          if (!material_map[std::string(f->material)]) {
	    Material *m = (Material*)malloc(sizeof(Material));
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
	    if (m->shininess>128.0f) m->shininess = 128.0f;
	    material_map[std::string(f->material)] = m;
	  }
          if (mat->texture1_map.name[0]) {
	    int texture_id = rend->requestTexture(mat->texture1_map.name);
	    if (current_texture == 0) ro->texture_id = current_texture = texture_id;
            if (texture_id != current_texture) {
	      current_texture = texture_id;
	      ro->num_points = v_counter / 3;
	      
              v_counter = n_counter = t_counter = 0;
	      
              ro = (RenderObject*)malloc(sizeof(RenderObject));
              memset(ro, 0, sizeof(RenderObject));
	      ro->texture_id = texture_id;
              ro->num_points = 3 * mesh->faces;
              ro->vertex_data = (float*)malloc(3 * ro->num_points * sizeof(float));
              ro->normal_data = (float*)malloc(3 * ro->num_points * sizeof(float));
              ro->texture_data = (mesh->texels) ? (float*)malloc(2 * ro->num_points * sizeof(float)) : (NULL);
	      render_objects.push_back(ro);
	    }
          }
        }     
        int i;
        for (i=0; i<3; ++i) {
	  float out[3];
	  lib3ds_vector_transform(out, M, mesh->pointL[f->points[i]].pos);
          out[0] -= d->pivot[0];
          out[1] -= d->pivot[1];
          out[2] -= d->pivot[2];
	  lib3ds_vector_transform(&ro->vertex_data[v_counter], node->matrix, out);
	  v_counter += 3;

          /* It is very likely the normals have been completely messed up by these transformations */
	  
	  lib3ds_vector_transform(out, M,  normalL[3 * p * i]);
          out[0] -= d->pivot[0];
          out[1] -= d->pivot[1];
          out[2] -= d->pivot[2];
	  lib3ds_vector_transform(&ro->normal_data[n_counter], node->matrix, out);
	  n_counter += 3;

	  if (mesh->texels) {
            ro->texture_data[t_counter++] = mesh->texelL[f->points[i]][0];
            ro->texture_data[t_counter++] = mesh->texelL[f->points[i]][1];
	  }

        }
      }
      ro->num_points = v_counter / 3;
      free(normalL);
      return;
      node->user.d = glGenLists(1);
      glNewList(node->user.d, GL_COMPILE);
      current_texture = 0;
      std::string current_material = "";
      glPushMatrix();
      for (std::list<RenderObject*>::const_iterator I = render_objects.begin(); I != render_objects.end(); I++) {
        RenderObject *ro = *I;
	if (std::string(ro->material_name) != current_material) {
	  Material *m = material_map[ro->material_name];
	  if (m) {
              rend->setMaterial(m->ambient, m->diffuse, m->specular, m->shininess, NULL);
            current_material = ro->material_name;
	  }	    
	}
	if (ro) {
          if (current_texture != ro->texture_id) {
            if (ro->texture_data) rend->switchTexture(ro->texture_id);
            current_texture = ro->texture_id;
          }
          rend->renderArrays(Graphics::RES_TRIANGLES, 0, ro->num_points, ro->vertex_data, ro->texture_data, ro->normal_data);
	  if (ro->vertex_data) free(ro->vertex_data);
  	  if (ro->normal_data) free(ro->normal_data);
	  if (ro->texture_data) free(ro->texture_data);
	  free(ro);
	}
      }
      glPopMatrix();
      glEndList();
    }
    if (node->user.d) {

      glPushMatrix();
      glTranslatef(_offset[0], _offset[1], _offset[2]);
      if (_scale != 1.0f) glScalef(_scale, _scale, _scale);
      glCallList(node->user.d);
      glPopMatrix();
    }
  }
}

} /* namespace Sear */
