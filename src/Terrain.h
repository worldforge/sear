// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Terrain.h,v 1.6 2002-09-08 00:24:53 simon Exp $

#ifndef SEAR_TERRAIN_H
#define SEAR_TERRAIN_H 1

namespace Sear {


class Terrain {

public:
  Terrain() {}
  virtual ~Terrain() {}

  virtual bool init() =0;
  virtual void shutdown()=0;
  virtual void draw()=0;
  virtual void update(float time_elapsed) =0;
  
  virtual float getHeight(float, float)=0;

  virtual void lowerDetail() =0;
  virtual void raiseDetail() =0;
  
  virtual void readConfig() = 0;
  virtual void writeConfig() = 0;
  
protected:
};

} /* namespace Sear */
#endif /* SEAR_TERRAIN_H */

