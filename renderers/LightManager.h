// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: LightManager.h,v 1.5 2006-05-17 23:15:35 alriddoch Exp $

#ifndef SEAR_LIGHTMANAGER_H
#define SEAR_LIGHTMANAGER_H 1

namespace Sear {

class Light;

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
