// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: CoreModelHandler.cpp,v 1.9 2005-03-15 17:55:04 simon Exp $


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
  m_initialised(false)
{}

CoreModelHandler::~CoreModelHandler() {
  assert(m_initialised == false);
//  if (m_initialised) shutdown();
}
	
int CoreModelHandler::init() {
  assert(m_initialised == false);
//  if (_initialised) shutdown();

  m_initialised = true;
  return 0;
}

int CoreModelHandler::shutdown() {
  assert(m_initialised == true);
  while (!m_core_models.empty()) {
    Cal3dCoreModel *core_model = m_core_models.begin()->second;
    assert(core_model && "Core model is NULL");
    core_model->shutdown();
    delete core_model;
    m_core_models.erase(m_core_models.begin());
  }
  m_initialised = false;
  return 0;
}

Cal3dModel *CoreModelHandler::instantiateModel(const std::string &filename) {
  assert(m_initialised && "CoreModelHandler not initialised");
  // Check to see if we have this core model loaded?
  CoreModelMap::iterator I = m_core_models.find(filename);
  Cal3dCoreModel *core_model;
  if (I != m_core_models.end()) {
    core_model = I->second;
  } else {
    // load core model
    core_model = new Cal3dCoreModel();
    core_model->init(filename);
    m_core_models[filename] = core_model;
  }
  assert(core_model && "Core model is NULL");
  // Instantiate and return a model
  return core_model->instantiate();
}

} /* namespace Sear */
