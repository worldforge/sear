// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: CoreModelHandler.cpp,v 1.1 2003-03-04 23:28:46 simon Exp $


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
  while (!core_models.empty()) {
    Cal3dCoreModel *core_model = *_core_models.begin();
    assert(core_model && "Core model is NULL");
    core_model->shutdown();
    delete core_model;
    _core_models.erase(_core_models.begin());
  }
  _initialised = false;
}

Cal3dModel *CoreModelHandler::instantiateModel(const std::string &filename) {
  // Check to see if we have this core model loaded?
  if (_core_models[filename] == NULL) {
    // load core model
    Cal3dCoreModel *core_model = new Cal3dCoreModel();
    core_model->init(filename);
    _core_models[filename] = core_model;
  }
  // Get core model
  Cal3dCoreModel *core_model = _core_models[filename];
  // Instantiate a model
  Cal3dModel *model = core_model->instantiate();
  // return model
  return model;
}

} /* namespace Sear */
