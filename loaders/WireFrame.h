// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: WireFrame.h,v 1.4 2002-09-07 23:27:06 simon Exp $

#ifndef SEAR_WIREFRAME_H
#define SEAR_WIREFRAME_H 1

#include <wfmath/axisbox.h>

#include "src/Model.h"

namespace Sear {

class WireFrame : public Model{
public:
  WireFrame(Render*);
  ~WireFrame();
  
  bool init(WFMath::AxisBox<3>);
  void shutdown();
  
  void render(bool);
  bool useTextures() { return false; }
  
private:
  static const int _num_points = 32;
  float _vertex_data[_num_points][3];
  bool _initialised;
};

} /* namespace Sear */
#endif /* SEAR_WIREFRAME_H */
