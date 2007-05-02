// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: WireFrame.h,v 1.14 2007-05-02 20:47:54 simon Exp $

#ifndef SEAR_WIREFRAME_H
#define SEAR_WIREFRAME_H 1

#include "common/types.h"

#include "Model.h"

namespace Sear {

class WireFrame : public Model{
public:
  WireFrame();
  virtual ~WireFrame();
  
  int init(const WFMath::AxisBox<3> &bbox);
  virtual int shutdown();
  virtual bool isInitialised() const { return m_initialised;  }
 
  virtual void contextCreated();
  virtual void contextDestroyed(bool check);
 
  virtual void render(bool);
  
private:
  static const int m_num_points = 32;
  bool m_initialised;
  unsigned int m_disp;
  Vertex_3 m_vertex_data[m_num_points];
};

} /* namespace Sear */
#endif /* SEAR_WIREFRAME_H */
