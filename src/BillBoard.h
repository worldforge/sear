// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_ 1

#include "Models.h"

namespace Sear {

class BillBoard : public Models {
public:
  BillBoard(float, float);
  ~BillBoard();
  
  bool init();
  void shutdown();
  
  const int getNumPoints() { return _num_points; }  
  const float *getVertexData() { return &_vertex_data[0][0]; }
  const float *getTextureData() { return &_texture_data[0][0]; }
  const float *getNormalData() { return &_normal_data[0][0]; }

  const bool hasVertexData() { return true; }
  const bool hasTextureData() { return true; }
  const bool hasNormalData() { return true; } 

  const Type getType() { return QUADS; }
  
private:
  static const int _num_points = 4;
  float _width;
  float _height;
  float _vertex_data[_num_points][3];
  float _texture_data[_num_points][2];
  float _normal_data[_num_points][3];
};

} /* namespace Sear */
#endif /* _BILLBOARD_H_ */
