// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: LightManager.cpp,v 1.2 2003-03-06 23:50:38 simon Exp $

#include "LightManager.h"
#include "Light.h"

#include "src/System.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

namespace Sear {

LightManager::LightManager() :
  _initialised(false)
{}

LightManager::~LightManager() {
  if (_initialised) shutdown();
}
  
void LightManager::init() {
  if (_initialised) shutdown();
  readConfig();
  _config_connection = System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &LightManager::config_update));
  _initialised = true;
}

void LightManager::shutdown() {
  writeConfig();
  _config_connection.disconnect();
  _initialised = false;
}

void LightManager::readConfig() {
  
}

void LightManager::writeConfig() {

}

void LightManager::config_update(const std::string &section, const std::string &key, varconf::Config &config){
}
  
void LightManager::update(float time_elapsed) {

}

} /* namespace Sear */

