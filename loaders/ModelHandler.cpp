// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: ModelHandler.cpp,v 1.11 2005-04-14 10:37:13 simon Exp $

#include <set>
#include <string.h>

#include <Atlas/Message/Element.h>

#include <varconf/Config.h>
#include "src/Console.h"
#include "src/Exception.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"
#include "src/System.h"
#include "src/WorldEntity.h"

#include "ModelHandler.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ModelSystem.h"
#include "3ds_Loader.h"
#include "cal3d/Cal3d_Loader.h"
#include "BoundBox_Loader.h"
#include "NPlane_Loader.h"
#include "WireFrame_Loader.h"
#include "LibModelFile_Loader.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

static const std::string CMD_LOAD_MODEL_RECORDS = "load_model_records";

ModelHandler::ModelHandler() :
  m_initialised(false),
  m_timeout(NULL)
{
  m_model_records.sigsv.connect(SigC::slot(*this, &ModelHandler::varconf_callback));
  m_model_records.sige.connect(SigC::slot(*this, &ModelHandler::varconf_error_callback));
}

ModelHandler::~ModelHandler() {
  assert (m_initialised == false);
}

void ModelHandler::init() {
  assert (m_initialised == false);

  m_timeout = new Eris::Timeout("sear_free_models", this, 60000);
  m_timeout->Expired.connect(SigC::slot(*this, &ModelHandler::TimeoutExpired));

  // Add default record
  varconf::Config &m_model_records = ModelSystem::getInstance().getModelRecords();
  m_model_records.setItem("default", ModelRecord::MODEL_LOADER, "wireframe");
  m_model_records.setItem("default", ModelRecord::STATE, "default");
  m_model_records.setItem("default", ModelRecord::SELECT_STATE, "select");
  m_model_records.setItem("default", ModelRecord::OUTLINE, false);

  m_initialised = true;
}

void ModelHandler::shutdown() {
  assert (m_initialised == true);

  delete m_timeout;

  // Clean up model loaders
  while (!m_model_loaders.empty()) {
    ModelLoader *ml = m_model_loaders.begin()->second;
    if (ml) delete (ml);
    m_model_loaders.erase(m_model_loaders.begin());
  }
  // Delete all unique records
  while (!m_object_map.empty()) {
    ModelRecord *mr = m_object_map.begin()->second;
    if (mr) {
      if (!mr->model_by_type) { // If model_by_type then record is not unique
        if (mr->model) {
          mr->model->shutdown();
          delete mr->model;
          mr->model = NULL;
        }
        delete mr;
      }
    }
    m_object_map.erase(m_object_map.begin());
  }
  // Delete all remaining records
  while (!m_model_records_map.empty()) {
    ModelRecord *mr = m_model_records_map.begin()->second;
    if (mr) {
      if (mr->model) {
	mr->model->shutdown();
        delete mr->model;
        mr->model = NULL;
      }
      delete mr;
    }
    m_model_records_map.erase(m_model_records_map.begin());
  }
  m_initialised = false;
}
  
ModelRecord *ModelHandler::getModel(Render *render, ObjectRecord *record, const std::string &model_id, WorldEntity *we) {
  assert (m_initialised == true);
  assert(record);
  // Model loaded for this object?
  if (m_object_map[record->id + model_id]) {
    return m_object_map[record->id + model_id];
  }


  if (m_model_records_map.find(model_id) != m_model_records_map.end()) {
    m_object_map[record->id + model_id] = m_model_records_map[model_id];
    return m_model_records_map[model_id];
  }
//  assert(render);
  // No existing model found, load up a new one
  if (!render) {
    std::cerr << "renderer is null" << std::endl;	  
    return NULL;
  }

  ModelRecord *model = NULL;
  varconf::Config &m_model_records = ModelSystem::getInstance().getModelRecords();
  std::string model_loader = (std::string)m_model_records.getItem(model_id, ModelRecord::MODEL_LOADER);

  if (model_loader.empty()) {
    printf("Model Loader not defined. Using BoundBox.\n");
    model_loader = "boundbox";
  }
  if (m_model_loaders.find(model_loader) == m_model_loaders.end()) {
    printf("Unknown Model Loader. Using BoundBox.\n");
    model_loader = "boundbox";
  }
  assert(m_model_loaders[model_loader] != NULL);
  if (m_model_loaders[model_loader]) model = m_model_loaders[model_loader]->loadModel(render, record, model_id, m_model_records);
  else {
    std::cerr << "No loader found: " << model_loader << std::endl;
    return NULL;
  }
  // Check model was loaded
  if (!model) {
    std::cerr << "Error loading model" << std::endl;	 
    return NULL;
  }
  if (we != NULL) {
    if (we->hasAttr("mode")) {
      model->model->action(we->valueOfAttr("mode").asString());
    }
    if (we->hasAttr("guise")) {
      Atlas::Message::MapType mt = we->valueOfAttr("guise").asMap();
      model->model->setAppearance(mt);
    }                                                                          
  }
	  
  // If model is a generic one, add it to the generic list
  if (model->model_by_type) m_model_records_map[model_id] = model;
  m_object_map[record->id + model_id] = model;
  return model; 
}

void ModelHandler::registerModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  assert (m_initialised == true);
  assert(model_type.empty() == false);
  assert(model_loader != NULL);

  // Check for bad values	
  if (model_type.empty()) throw Exception("No type specified");
  if (!model_loader) throw Exception("No model loader given");

  // Throw error if we already have a loader for this type
  assert(m_model_loaders[model_type] ==  NULL);

  // If all is well, assign loader
  m_model_loaders[model_type] = model_loader;
}

void ModelHandler::unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  assert (m_initialised == true);
  // Only unregister a model laoder if it is properly registered
  if (m_model_loaders[model_type] == model_loader) m_model_loaders[model_type] = NULL;
}

void ModelHandler::checkModelTimeouts() {
  assert (m_initialised == true);

  // TODO what about records with no model?
  if (debug) std::cout << "Checking Timeouts" << std::endl;

  // Loop through and find all objects that have expired and add to set object
  std::set<ModelRecord*> expired_set;
  for (ModelRecordMap::iterator I = m_model_records_map.begin(); I != m_model_records_map.end(); ++I) {
    ModelRecord *record = I->second;
    assert(record);
    if (record) {
      Model *model = record->model;
      if (model) {
        if ((System::instance()->getTimef() - model->getLastTime()) > 60.0f) {
          // Add to set object
          expired_set.insert(record);
          // Remove record from list
          m_model_records_map[I->first] = NULL;
        }
      }
    }
  }
  // Do the same again for the object map
  for (ObjectRecordMap::iterator I = m_object_map.begin(); I != m_object_map.end(); ++I) {
    ModelRecord *record = I->second;
    assert(record);
    if (record) {
      Model *model = record->model;
      if (model) {
        if ((System::instance()->getTimef() - model->getLastTime()) > 60.0f) {
          expired_set.insert(record);
          m_object_map[I->first] = NULL;
        }
      }
    }
  }

  // Unload old objects
  while (!expired_set.empty()) {
    ModelRecord *record = *expired_set.begin();
    assert(record);
    if (record) {
      if (debug) std::cout << "Unloading: " << record->id << std::endl;
      Model *model = record->model;
      if (model) delete model;
      delete record;
    }
    expired_set.erase(expired_set.begin());
  }
}

void ModelHandler::TimeoutExpired() {
  checkModelTimeouts();
  m_timeout->reset(60000);
}

void ModelHandler::loadModelRecords(const std::string &filename) {
  m_model_records.readFromFile(filename);
}

void ModelHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  // Convert textual state name to the state number
  if (key == "state") {
//    printf("Setting State Num: %d\n",  RenderSystem::getInstance().requestState(config.getItem(section, key)));
    int stn = RenderSystem::getInstance().requestState(config.getItem(section, key));
    assert(stn);
    config.setItem(section, "state_num", stn);
  }
  else if (key == "select_state") {
    config.setItem(section, "select_state_num", RenderSystem::getInstance().requestState(config.getItem(section, key)));
  }
}

void ModelHandler::varconf_error_callback(const char *message) {
  std::cerr << message << std::endl;
}

void ModelHandler::registerCommands(Console *console) {
  assert(m_initialised == true);
  assert(console != NULL);
  
  console->registerCommand(CMD_LOAD_MODEL_RECORDS, this);
}

void ModelHandler::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_LOAD_MODEL_RECORDS) {
    loadModelRecords(args);
  }

}

void ModelHandler::invalidate() {
  // Do the same again for the object map
  //for (ObjectRecordMap::iterator I = m_object_map.begin(); I != m_object_map.end(); ++I) {
  for (ModelRecordMap::iterator I = m_model_records_map.begin(); I != m_model_records_map.end(); ++I) {
    ModelRecord *record = I->second;
    assert(record);
    Model *model = record->model;
    if (model) {
      model->invalidate();
    }
  }


}

} /* namespace Sear */
