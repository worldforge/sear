// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall
 
#ifndef _SKYDOME_H_
#define _SKYDOME_H_ 1

#include "Sky.h"

namespace Sear {

class SkyDome : public Sky {
	
public:
  SkyDome();
  ~SkyDome();
  
  bool init();
  void shutdown();
  void update(float time_elapsed);
  
  int getHRes();
  int getVRes();
  
  float *getVertexData();
  float *getTextureData();
  float *getNormalData();
  
  bool hasVertexData();
  bool hasTextureData();
  bool hasNormalData();

protected:
  float **vertex_coords;
  int **texture_coords;
  float **normal_coords;
};

}

#endif /* _SKYDOME_H_ */
