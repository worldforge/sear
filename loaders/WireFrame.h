// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: WireFrame.h,v 1.9 2006-01-28 15:35:49 simon Exp $

#ifndef SEAR_WIREFRAME_H
#define SEAR_WIREFRAME_H 1

#include <wfmath/axisbox.h>

#include "common/types.h"

#include "Model.h"

namespace Sear {

class WireFrame : public Model{
public:
  WireFrame(Render*);
  ~WireFrame();
  
  int init(WFMath::AxisBox<3>);
  int shutdown();
 
  void contextCreated();
  void contextDestroyed(bool check);
 
  void render(bool);
  
private:
  static const int m_num_points = 32;
  bool m_initialised;
  int m_disp;
  Vertex_3 m_vertex_data[m_num_points];
};

} /* namespace Sear */
#endif /* SEAR_WIREFRAME_H */
