// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2002 Simon Goodall

#include "3ds.h"
#include "3dsMesh.h"

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include "Log.h"

#include "Utility.h"

#include <GL/gl.h>

namespace Sear {

GLuint model_list = 0;

Model_3ds::Model_3ds() :
  _num_models(0),
  _models(NULL)
{

}

Model_3ds::~Model_3ds() {


}

bool Model_3ds::init() {
  return true;
}

void Model_3ds::shutdown() {

}

bool Model_3ds::loadModel(const std::string &file_name) {
  Log::writeLog(std::string("Loading 3ds model: ") + file_name,Log::INFO);
  // Load 3ds file
  Lib3dsFile* model = lib3ds_file_load(file_name.c_str());
  if (!model) {
    Log::writeLog(std::string("Unable to load ") + file_name, Log::ERROR);
    return false;
  }
  // Calculate initial positions
  lib3ds_file_eval(model,0);
  Lib3dsMesh *mesh;
  // Get number of meshes
  for (mesh=model->meshes; mesh!=0; mesh=mesh->next) _num_models++;

  _models = (Model_3dsMesh**)malloc(_num_models * sizeof(Model_3dsMesh*));
  if (!_models) {
     _num_models = 0;
     Log::writeLog("3ds: Error allocating memory for model", Log::ERROR);
     return false;   
  }
  unsigned int mesh_number = 0;
  for (mesh=model->meshes; mesh!=0; mesh=mesh->next, mesh_number++) {
//    float vertex_data[mesh->faces*3][3];
    float *vertex_data = (float *)malloc(mesh->faces * 3 * 3 * sizeof(float));
    unsigned int counter = 0;
    for (unsigned int p = 0; p < mesh->faces; ++p) {
      Lib3dsFace *f=&mesh->faceL[p];
      for (unsigned int i = 0; i < 3; i++) {
        vertex_data[counter++] = mesh->pointL[f->points[i]].pos[0];
        vertex_data[counter++] = mesh->pointL[f->points[i]].pos[1];
        vertex_data[counter++] = mesh->pointL[f->points[i]].pos[2];
      }
    }   
    _models[mesh_number] = new Model_3dsMesh();
    _models[mesh_number]->init(mesh->faces * 3, vertex_data, NULL, NULL);
  }
  lib3ds_file_free(model);
  return true;
}
 
void Model_3ds::render() {
}

Models *Model_3ds::getModel(unsigned int index) {
  if (!_models) {
     Log::writeLog("3ds - Error no models!", Log::ERROR);   
     return NULL;
  }
  if (index >= _num_models) {
    Log::writeLog("3ds - Index out of bounds", Log::ERROR);
    return NULL;
  }
  return _models[index];
}
} /* namespace Sear */
