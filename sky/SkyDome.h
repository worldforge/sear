// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall
 
#ifndef _SKYDOME_H_
#define _SKYDOME_H_ 1

#include "../src/Sky.h"

namespace Sear {

class SkyDome : public Sky {
	
public:
  SkyDome();
  ~SkyDome();
  
  bool init();
  void shutdown();
  void update(float time_elapsed);
  
protected:
};

}

#endif /* _SKYDOME_H_ */
