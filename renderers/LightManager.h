// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: LightManager.h,v 1.2 2005-06-16 14:06:14 simon Exp $

#ifndef SEAR_LIGHTMANAGER_H
#define SEAR_LIGHTMANAGER_H 1

//#include <sigc++/object_slot.h>
//#include <varconf/Config.h>

//#include <vector>
//#include <stack>
//#include <queue>

#include "Light.h"

namespace Sear {

class LightManager {
public:
	
  LightManager();
  ~LightManager();

  int init();
  void shutdown();

  void reset();
  void applyLight(const Light &light); 

private:
  bool m_initialised;
  int m_totalLights;
  int m_lightCounter;
};
	
} /* namespace Sear */

#endif /* SEAR_LIGHTMANAGER_H */
