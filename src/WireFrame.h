// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _WIREFRAME_H_
#define _WIREFRAME_H_ 1

#include "Models.h"
#include <wfmath/axisbox.h>

namespace Sear {

class WireFrame : public Models{
public:
  WireFrame(WFMath::AxisBox<3>);
  ~WireFrame();
  
  bool init();
  void shutdown();
  
  const int getNumPoints() const { return _num_points; }  
  const float *getVertexData() const { return &_vertex_data[0][0]; }
  const float *getTextureData() const { return NULL; }
  const float *getNormalData() const { return NULL; }

  const bool hasVertexData() const { return true; }
  const bool hasTextureData() const { return false; }
  const bool hasNormalData() const { return false; }
  
  const Type getType() const { return LINES; }
  
private:
  static const int _num_points = 32;
  WFMath::AxisBox<3> _bbox;
  float _vertex_data[_num_points][3];
};

} /* namespace Sear */
#endif /* _WIREFRAME_H_ */
