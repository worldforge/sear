// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2002 Simon Goodall

#ifndef _3DSMESH_H_
#define _3DSMESH_H_ 1

#include "Models.h"

namespace Sear {

class Model_3dsMesh : public Models {
public:
  Model_3dsMesh();
  virtual ~Model_3dsMesh();

  bool init(int, float*, float*, float*);
  void shutdown();

  const int getNumPoints() { return _num_points; }
  const float *getVertexData() { return _vertex_data; }
  const float *getTextureData() { return _texture_data; }
  const float *getNormalData() { return _normal_data; }

  const bool hasVertexData() { return true; }
  const bool hasTextureData() { return false; }
  const bool hasNormalData() { return false; } 

  const Type getType() { return TRIANGLES; }
  
protected:
  int _num_points;
  float *_vertex_data;
  float *_texture_data;
  float *_normal_data;
};

} /* namespace Sear */

#endif /* _3DSMESH_H_ */
