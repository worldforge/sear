// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: NPlane.h,v 1.14 2006-02-16 15:59:01 simon Exp $

#ifndef SEAR_NPLANE_H
#define SEAR_NPLANE_H 1

#include <string>

#include "common/types.h"
#include "common/SPtr.h"

#include "Model.h"

namespace Sear {

class StaticObject;
	
class NPlane : public Model {
public:
  NPlane();
  virtual ~NPlane();
  
  int init(const std::string &, unsigned int num_planes, float width, float height);
  virtual int shutdown();

  virtual bool isInitialised() const { return m_initialised; }
 
  virtual void contextCreated();
  virtual void contextDestroyed(bool check);
 
  virtual void render(bool); 

  virtual RotationStyle rotationStyle() { return ROS_POSITION; }
  
private:
  bool m_initialised;
  SPtrShutdown<StaticObject> m_so; 
};

} /* namespace Sear */ 
#endif /* SEAR_NPLANE_H */
