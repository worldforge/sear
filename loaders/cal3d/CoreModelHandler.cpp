// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: CoreModelHandler.cpp,v 1.8 2004-04-27 15:07:02 simon Exp $


#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "CoreModelHandler.h"
#include "Cal3dCoreModel.h"
#include "Cal3dModel.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


namespace Sear {
	
CoreModelHandler::CoreModelHandler() :
  _initialised(false)
{}

CoreModelHandler::~CoreModelHandler() {
  if (_initialised) shutdown();
}
	
void CoreModelHandler::init() {
  if (_initialised) shutdown();

  _initialised = true;
}

void CoreModelHandler::shutdown() {
  while (!_core_models.empty()) {
    Cal3dCoreModel *core_model = _core_models.begin()->second;
    assert(core_model && "Core model is NULL");
    core_model->shutdown();
    delete core_model;
    _core_models.erase(_core_models.begin());
  }
  _initialised = false;
}

Cal3dModel *CoreModelHandler::instantiateModel(const std::string &filename) {
  assert(_initialised && "CoreModelHandler not initialised");
  // Check to see if we have this core model loaded?
  CoreModelMap::iterator I = _core_models.find(filename);
  Cal3dCoreModel *core_model;
  if (I != _core_models.end()) {
    core_model = I->second;
  } else {
//    std::cout << "Loading Core Model" << std::endl << std::flush;
    // load core model
    core_model = new Cal3dCoreModel();
    core_model->init(filename);
    _core_models[filename] = core_model;
  }
  assert(core_model && "Core model is NULL");
  // Instantiate and return a model
  return core_model->instantiate();
}

} /* namespace Sear */
