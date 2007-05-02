// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

// $Id: CoreModelHandler.cpp,v 1.15 2007-05-02 20:47:54 simon Exp $

#include "CoreModelHandler.h"
#include "Cal3dCoreModel.h"
#include "Cal3dModel.h"

namespace Sear {
	
CoreModelHandler::CoreModelHandler() :
  m_initialised(false)
{}

CoreModelHandler::~CoreModelHandler() {
  if (m_initialised) shutdown();
}

int CoreModelHandler::init() {
  assert(m_initialised == false);

  m_initialised = true;
  return 0;
}

int CoreModelHandler::shutdown() {
  assert(m_initialised == true);

  m_core_models.clear();

  m_initialised = false;
  return 0;
}

Cal3dModel *CoreModelHandler::instantiateModel(const std::string &filename) {
  assert(m_initialised && "CoreModelHandler not initialised");
  // Check to see if we have this core model loaded?
  CoreModelMap::iterator I = m_core_models.find(filename);
  SPtr<Cal3dCoreModel> core_model;
  if (I != m_core_models.end()) {
    core_model = I->second;
  } else {
    // load core model
    core_model = SPtr<Cal3dCoreModel>(new Cal3dCoreModel());
    if (core_model->init(filename)) {
      printf("Error initialising model %s\n", filename.c_str());
      core_model.release();
      return NULL;
    }
    m_core_models[filename] = core_model;
  }
  assert(core_model.get() && "Core model is NULL");
  // Instantiate and return a model
  return core_model->instantiate();
}

} /* namespace Sear */
