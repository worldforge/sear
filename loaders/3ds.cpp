// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2002 Simon Goodall

#include <list>
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

#include <iostream>


typedef struct {
  float *vertex_data;
  float *normal_data;
  float *texture_data;
  unsigned int num_points;
  int texture_id;
} RenderObject;


namespace Sear {
void render_node(Lib3dsNode * node, Lib3dsFile * file);

ThreeDS::ThreeDS() : model (NULL)
{

}

ThreeDS::~ThreeDS() {

}

bool ThreeDS::init(const std::string &file_name) {
  // Load 3ds file
  Log::writeLog(std::string("Loading: ") + file_name, Log::LOG_DEFAULT);
  model = lib3ds_file_load(file_name.c_str());
  if (!model) {
    Log::writeLog(std::string("Unable to load ") + file_name, Log::LOG_ERROR);
    return false;
  }
  // Calculate initial positions
  lib3ds_file_eval(model,1);
  return true;
}

void ThreeDS::shutdown() {
  if (model) {
    lib3ds_file_free(model);
    model = NULL;
  }
}

void ThreeDS::render(bool) {
  if (model) {
    Lib3dsNode *p;
    if (model->nodes == 0) {
      cerr << "BUFFER" << endl;
      cout << model->meshes << endl;
    }
    for (p=model->nodes; p!=0; p=p->next) {
      render_node(p, model);
    }
  } else {
    Log::writeLog(std::string("Unable to render model"), Log::LOG_ERROR);
  }
}


void render_node(Lib3dsNode *node, Lib3dsFile *file) {
  static Render *rend = System::instance()->getGraphics()->getRender();
  std::list<RenderObject*> render_objects;
  Lib3dsNode *p;
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
        }
        if (mat) {
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
/*		
          float x = mesh->pointL[f->points[i]].pos[0];
          float y = mesh->pointL[f->points[i]].pos[1];
          float z = mesh->pointL[f->points[i]].pos[2];
          ro->vertex_data[v_counter++] = M[0][0] * x + M[0][1] * y + M[0][2] * z;
          ro->vertex_data[v_counter++] = M[1][0] * x + M[1][1] * y + M[1][2] * z;
          ro->vertex_data[v_counter++] = M[2][0] * x + M[2][1] * y + M[2][2] * z;
          x = normalL[3 * p * i][0];
          y = normalL[3 * p * i][1];
          z = normalL[3 * p * i][2];
          ro->normal_data[n_counter++] = M[0][0] * x + M[0][1] * y + M[0][2] * z;
          ro->normal_data[n_counter++] = M[1][0] * x + M[1][1] * y + M[1][2] * z;
          ro->normal_data[n_counter++] = M[2][0] * x + M[2][1] * y + M[2][2] * z;
          ro->texture_data[t_counter++] = mesh->texelL[f->points[i]][0];
          ro->texture_data[t_counter++] = mesh->texelL[f->points[i]][1];
*/

          ro->vertex_data[v_counter++] = mesh->pointL[f->points[i]].pos[0];
          ro->vertex_data[v_counter++] = mesh->pointL[f->points[i]].pos[1];
          ro->vertex_data[v_counter++] = mesh->pointL[f->points[i]].pos[2];
	  
          ro->normal_data[n_counter++] = normalL[3 * p * i][0];
          ro->normal_data[n_counter++] = normalL[3 * p * i][1];
          ro->normal_data[n_counter++] = normalL[3 * p * i][2];
	  if (mesh->texels) {
            ro->texture_data[t_counter++] = mesh->texelL[f->points[i]][0];
            ro->texture_data[t_counter++] = mesh->texelL[f->points[i]][1];
	  }

        }
      }
      ro->num_points = v_counter / 3;
      free(normalL);
      node->user.d = glGenLists(1);
      glNewList(node->user.d, GL_COMPILE);
      current_texture = 0;
      glColor3f(1.0f, 1.0f, 1.0f);
      glPushMatrix();
      glMultMatrixf(&M[0][0]);
      for (std::list<RenderObject*>::const_iterator I = render_objects.begin(); I != render_objects.end(); I++) {
        RenderObject *ro = *I;
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
      Lib3dsObjectData *d;

      glPushMatrix();
//      glScalef(0.05f, 0.05f, 0.05f);
      d=&node->data.object;
      glMultMatrixf(&node->matrix[0][0]);
      glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glCallList(node->user.d);
      glPopMatrix();
    }
  }
}

} /* namespace Sear */
