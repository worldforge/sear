// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2002 Simon Goodall


#include "3dsMesh.h"

namespace Sear {

Model_3dsMesh::Model_3dsMesh() :
  _num_points(0),
  _vertex_data(NULL),
  _texture_data(NULL),
  _normal_data(NULL)
{}

Model_3dsMesh::~Model_3dsMesh() {}

bool Model_3dsMesh::init(int num_points, float *vertex_data, float *texture_data, float *normal_data) {
  _num_points = num_points;
  // TODO CHECK FOR NULL ENTRIES AND UPDATE ACCES CHECKS ACCORDINGLY
  _vertex_data = vertex_data;
  _texture_data = texture_data;
  _normal_data = normal_data;
  return true;
}

void Model_3dsMesh::shutdown() {
  _num_points = 0;
  if (_vertex_data) delete _vertex_data;
  if (_texture_data) delete _texture_data;
  if (_normal_data) delete _normal_data;
}


} /* namespace Sear */
