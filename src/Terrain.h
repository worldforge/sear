// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _TERRAIN_H_
#define _TERRAIN_H_ 1

#include <stdlib.h>

namespace Sear {


class Terrain {

public:
  Terrain() {}
  virtual ~Terrain() {}

  virtual bool init() =0;
  virtual void shutdown()=0;
  virtual void draw()=0;
  virtual void update(float time_elapsed) =0;

//  virtual int getMapWidth() =0;
//  virtual int getMapHeight() =0;
  //Assuming square map
//  virtual int getMapSize() =0;

  
  virtual float getHeight(float, float)=0;
//  int ground_id; 

  virtual void lowerDetail() =0;
  virtual void raiseDetail() =0;
  
  virtual void readConfig() = 0;
  virtual void writeConfig() = 0;
  
protected:
};

} /* namespace Sear */
#endif /* _TERRAIN_H_ */

