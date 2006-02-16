// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: LightManager.h,v 1.4 2006-02-16 17:45:20 simon Exp $

#ifndef SEAR_LIGHTMANAGER_H
#define SEAR_LIGHTMANAGER_H 1

#include "Light.h"

namespace Sear {

class LightManager {
public:
	
  LightManager();
  ~LightManager();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void reset();
  void applyLight(const Light &light); 

private:
  bool m_initialised;
  int m_totalLights;
  int m_lightCounter;
};
	
} /* namespace Sear */

#endif /* SEAR_LIGHTMANAGER_H */
