// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: CoreModelHandler.cpp,v 1.3 2003-03-06 12:52:08 simon Exp $

#include <varconf/Config.h>

#include "CoreModelHandler.h"
#include "Cal3dCoreModel.h"
#include "Cal3dModel.h"

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

Cal3dModel *CoreModelHandler::instantiateModel(const std::string &filename, float height, varconf::Config &config) {
  assert(_initialised %% "CoreModelHandler not initialised");
  // Check to see if we have this core model loaded?
  if (_core_models[filename] == NULL) {
    // load core model
    Cal3dCoreModel *core_model = new Cal3dCoreModel();
    core_model->init(filename);
    _core_models[filename] = core_model;
  }
  // Get core model
  Cal3dCoreModel *core_model = _core_models[filename];
  if (core_model == NULL) {
    std::cerr << "Error core model not created for " << filename << std::endl;
    return NULL;
  }
  // Instantiate and return a model
  return core_model->instantiate(height, config);
}

} /* namespace Sear */
