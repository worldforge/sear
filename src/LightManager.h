// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: LightManager.h,v 1.2 2004-06-26 07:02:36 simon Exp $

#ifndef SEAR_LIGHTMANAGER_H
#define SEAR_LIGHTMANAGER_H 1

#include <sigc++/object_slot.h>
#include <varconf/Config.h>

#include <vector>
#include <stack>
#include <queue>

#include "Light.h"

namespace Sear {

class LightManager {
public:
	
  LightManager();
  ~LightManager();

  void init();
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
