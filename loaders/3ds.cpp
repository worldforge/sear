// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2002 Simon Goodall

#include <GL/gl.h>

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "3ds.h"

Lib3dsFile* model = NULL;
GLuint treemodel_list = 0;

namespace Sear {
void draw3dsFile(Lib3dsFile * node);

ThreeDS::ThreeDS()
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
  lib3ds_file_eval(model,0);
 /*
  Lib3dsMesh *mesh;
  // Get number of meshes
  for (mesh=model->meshes; mesh!=0; mesh=mesh->next) _num_models++;
  
  // Allocate memory for child models
  _models = (ThreeDSMesh**)malloc(_num_models * sizeof(ThreeDSMesh*));
  if (!_models) {
     _num_models = 0;
     Log::writeLog("3ds: Error allocating memory for model", Log::LOG_ERROR);
     return false;   
  }
  
  unsigned int mesh_number = 0;
  // Extract mesh data
  for (mesh=model->meshes; mesh!=0; mesh=mesh->next, mesh_number++) {
    // SHould perhaps replace memory allocation with an error on failure
    bool use_normals = true;
    Lib3dsVector *normalL = (Lib3dsVector *)malloc(3*sizeof(Lib3dsVector)*mesh->faces);
    if (normalL) lib3ds_mesh_calculate_normals(mesh, normalL);
    else use_normals = false;
  
    // Allocate memory for model data
    float *vertex_data = (float *)malloc(mesh->faces * 3 * 3 * sizeof(float));
    float *normal_data = (float *)malloc(mesh->faces * 3 * 3 * sizeof(float));
    float *texture_data = (float *)malloc(mesh->faces * 3 * 2 * sizeof(float));
    if (!(vertex_data && normal_data && texture_data)) {
      Log::writeLog("3ds: Error allocating memory for model mesh data", Log::LOG_ERROR);
      if (vertex_data) free(vertex_data);
      if (normal_data) free(normal_data);
      if (texture_data) free(texture_data);
      return false;
    }
    unsigned int v_counter = 0; // Vertex array position counter
    unsigned int n_counter = 0; // Normals array position counter
    unsigned int t_counter = 0; // Tex Coords array position counter
    // Loop though each face in the mesh
    for (unsigned int p = 0; p < mesh->faces; ++p) {
      Lib3dsFace *f=&mesh->faceL[p];
      // Loop through the three points in each face
      for (unsigned int i = 0; i < 3; i++) {
	// Add point data to arrays
        vertex_data[v_counter++] = mesh->pointL[f->points[i]].pos[0];
        vertex_data[v_counter++] = mesh->pointL[f->points[i]].pos[1];
        vertex_data[v_counter++] = mesh->pointL[f->points[i]].pos[2];

	if (use_normals) {
          normal_data[n_counter++] = normalL[3*p+i][0];
          normal_data[n_counter++] = normalL[3*p+i][1];
	  normal_data[n_counter++] = normalL[3*p+i][2];
	}
	
	if (mesh->texels > 0) {
          texture_data[t_counter++] = mesh->texelL[f->points[i]][0];
          texture_data[t_counter++] = mesh->texelL[f->points[i]][1];
	}
      }
    }   
    // Clean up unused data arrays
    if (!use_normals) {
      free(normal_data);
      normal_data = NULL;
    }
    if (mesh->texels == 0) {
      free(texture_data);
      texture_data = NULL;
    }
    // Create new model
    _models[mesh_number] = new ThreeDSMesh();
    // Assign data to model
    _models[mesh_number]->init(Model::TRIANGLES, mesh->faces * 3, vertex_data, texture_data, normal_data);
  }
  // Free file
  lib3ds_file_free(model);
  */
	  return true;
}

void ThreeDS::shutdown() {
  lib3ds_file_free(model);
}

void ThreeDS::render(bool) {
  draw3dsFile(model);
}


void draw3dsFile(Lib3dsFile * node)
{
  if (!node) {
    Log::writeLog("No model to render", Log::LOG_ERROR);
    return;
  }
  int num_meshes = 0;
  if (!treemodel_list) {
    Lib3dsMesh *mesh;
    for (mesh=node->meshes; mesh!=0; mesh=mesh->next) {

      treemodel_list=glGenLists(1);
      glNewList(treemodel_list, GL_COMPILE);

      {
        unsigned p;
        Lib3dsVector *normalL = (Lib3dsVector *)malloc(3*sizeof(Lib3dsVector)*mesh->faces);

        {
          Lib3dsMatrix M;
          lib3ds_matrix_copy(M, mesh->matrix);
          lib3ds_matrix_inv(M);
          glMultMatrixf(&M[0][0]);
        }
        lib3ds_mesh_calculate_normals(mesh, normalL);

        for (p = 0; p < mesh->faces; ++p) {
          Lib3dsFace *f=&mesh->faceL[p];
//          Lib3dsMaterial *mat=0;
//          GLint texture = -1;
//          if (f->material[0]) {
//            mat=lib3ds_file_material_by_name(model, f->material);
//          }else{
//            mat=node->materials;
//          }
//          if (mat) {
//            static GLfloat a[4]={0,0,0,1};
//            float s;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
//            s = pow(2, 10.0*mat->shininess);
//            if (s>128.0) {
//              s=128.0;
//            }
//            glMaterialf(GL_FRONT, GL_SHININESS, s);
//            if (mat->texture1_map.name[0]) {
//                texture = Texture::get(mat->texture1_map.name);
//            }
//          } else {
//            Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
//            Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
//            Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
//            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
//          }
          {
//            if (texture != -1) {
//                glEnable(GL_TEXTURE_2D);
//                glBindTexture(GL_TEXTURE_2D, texture);
//            }
            int i;
            glBegin(GL_TRIANGLES);
              glNormal3fv(f->normal);
              for (i=0; i<3; ++i) {
                glNormal3fv(normalL[3*p+i]);
//                if (texture != -1) {
//                    glTexCoord2fv(mesh->texelL[f->points[i]]);
//                }
                glVertex3fv(mesh->pointL[f->points[i]].pos);
              }
            glEnd();
//            if (texture != -1) {
//                glDisable(GL_TEXTURE_2D);
//            }
          }
        }

        free(normalL);
      }

      glEndList();
    }
  }

    if (treemodel_list) {
      Lib3dsObjectData *d;

      glPushMatrix();
      // d=&node->data.object;
      // glMultMatrixf(&node->matrix[0][0]);
      // glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glScalef(0.1f, 0.1f, 0.1f);
      glCallList(treemodel_list);
      /*glutSolidSphere(50.0, 20,20);*/
      glPopMatrix();
    }


}

} /* namespace Sear */
