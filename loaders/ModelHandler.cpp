// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: ModelHandler.cpp,v 1.2 2005-01-09 18:28:11 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <set>
#include <string.h>

#include <Atlas/Message/Element.h>

#include <varconf/Config.h>

#include "common/Log.h"

#include "src/Event.h"
#include "src/EventHandler.h"
#include "src/Exception.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"
#include "renderers/Render.h"
#include "src/System.h"
#include "src/WorldEntity.h"

#include "ModelHandler.h"
#include "ModelLoader.h"
#include "3ds_Loader.h"
#include "cal3d/Cal3d_Loader.h"
#include "BoundBox_Loader.h"
#include "NPlane_Loader.h"
#include "WireFrame_Loader.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

ModelHandler::ModelHandler() :
  m_initialised(false)
	
{
  // TODO: this is not the place
  // create all the model loaders
  new Cal3d_Loader(this);
  new BoundBox_Loader(this);
  new WireFrame_Loader(this);
  new NPlane_Loader(this);
  new ThreeDS_Loader(this);
}

ModelHandler::~ModelHandler() {
  if (m_initialised) shutdown();
}

void ModelHandler::init() {
  if (m_initialised) shutdown();
  // TODO another method would be better - or a clean up of the event system
  System::instance()->getEventHandler()->addEvent(Event(EF_FREE_MODELS, NULL, EC_TIME, 60000 + System::instance()->getTime()));
  // Add default record
  varconf::Config &model_config = System::instance()->getModelRecords();
  model_config.setItem("default", ModelRecord::MODEL_LOADER, "wireframe");
  model_config.setItem("default", ModelRecord::STATE, "default");
  model_config.setItem("default", ModelRecord::SELECT_STATE, "select");
  model_config.setItem("default", ModelRecord::OUTLINE, false);
  m_initialised = true;
}

void ModelHandler::shutdown() {
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
  while (!m_model_records.empty()) {
    ModelRecord *mr = m_model_records.begin()->second;
    if (mr) {
      if (mr->model) {
	mr->model->shutdown();
        delete mr->model;
        mr->model = NULL;
      }
      delete mr;
    }
    m_model_records.erase(m_model_records.begin());
  }
  m_initialised = false;
}
  
ModelRecord *ModelHandler::getModel(Render *render, ObjectRecord *record, const std::string &model_id, WorldEntity *we) {
  // Model loaded for this object?
  if (m_object_map[record->id + model_id]) return m_object_map[record->id + model_id];
  
  if (m_model_records[model_id]) {
    m_object_map[record->id + model_id] = m_model_records[model_id];
    return m_model_records[model_id];
  }
  // No existing model found, load up a new one
  if (!render) {
    std::cerr << "renderer is null" << std::endl;	  
    return NULL;
  }
  if (!record) {
    std::cerr << "record is NULL" << std::endl;
    return NULL;
  }
  ModelRecord *model = NULL;
  varconf::Config &model_config = System::instance()->getModelRecords();
  std::string model_loader = (std::string)model_config.getItem(model_id, ModelRecord::MODEL_LOADER);

  if (model_loader.empty()) {
    printf("Model Loader not defined. Using BoundBox.\n");
    model_loader = "boundbox";
  }
  if (m_model_loaders.find(model_loader) == m_model_loaders.end()) {
    printf("Unknown Model Loader. Using BoundBox.\n");
    model_loader = "boundbox";
  }
  if (m_model_loaders[model_loader]) model = m_model_loaders[model_loader]->loadModel(render, record, model_id, model_config);
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
    if (we->hasProperty("guise")) {
      Atlas::Message::Element::MapType mt = we->getProperty("guise").asMap();
      model->model->setAppearance(mt);
    }                                                                          
  }
	  
  // If model is a generic one, add it to the generic list
  if (model->model_by_type) m_model_records[model_id] = model;
  m_object_map[record->id + model_id] = model;
  return model; 
}

void ModelHandler::registerModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  // Check for bad values	
  if (model_type.empty()) throw Exception("No type specified");
  if (!model_loader) throw Exception("No model loader given");
  // Throw error if we already have a loader for this type
  // TODO: decide whether we should override existing loaders with new one.
  // TODO throw execption is not a good idea as this method gets called via a constructor
  //if (m_model_loaders[model_type]) throw Exception("Model loader already exists for this type!");
  // If all is well, assign loader
  m_model_loaders[model_type] = model_loader;
}

void ModelHandler::unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  // Only unregister a model laoder if it is properly registered
  if (m_model_loaders[model_type] == model_loader) m_model_loaders[model_type] = NULL;
}

void ModelHandler::checkModelTimeouts() {
  // TODO what about records with no model?
  if (debug) std::cout << "Checking Timeouts" << std::endl;
  std::set<ModelRecord*> expired_set;
  for (ModelRecordMap::iterator I = m_model_records.begin(); I != m_model_records.end(); ++I) {
    ModelRecord *record = I->second;
    if (record) {
      Model *model = record->model;
      if (model) {
        if ((System::instance()->getTimef() - model->getLastTime()) > 60.0f) {
          expired_set.insert(record);
          m_model_records[I->first] = NULL;
        }
      }
    }
  }
  for (ObjectRecordMap::iterator I = m_object_map.begin(); I != m_object_map.end(); ++I) {
    ModelRecord *record = I->second;
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
  while (!expired_set.empty()) {
    ModelRecord *record = *expired_set.begin();
    if (record) {
      if (debug) std::cout << "Unloading: " << record->id << std::endl;
      Model *model = record->model;
      if (model) delete model;
      delete record;
    }
    expired_set.erase(expired_set.begin());
  }
  System::instance()->getEventHandler()->addEvent(Event(EF_FREE_MODELS, NULL, EC_TIME, 60000 + System::instance()->getTime()));
}

} /* namespace Sear */
