// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: SkyDome.h,v 1.2 2002-09-08 00:24:53 simon Exp $

#ifndef SEAR_SKYDOME_H
#define SEAR_SKYDOME_H 1

#include "src/Sky.h"

namespace Sear {

class SkyDome : public Sky {
	
public:
  SkyDome();
  ~SkyDome();
  
  bool init();
  void shutdown();
  void update(float time_elapsed);
  
protected:
  bool _initialised;
};

}

#endif /* SEAR_SKYDOME_H */
