// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ModelHandler.cpp,v 1.25 2002-10-09 17:13:39 alriddoch Exp $

#include "System.h"
#include <set>
#include <string.h>

#include <varconf/Config.h>
//#include <Eris/TypeInfo.h>

#include "config.h"

#include "common/Log.h"
#include "loaders/3ds_Loader.h"
#include "loaders/Cal3d_Loader.h"
#include "loaders/BoundBox_Loader.h"
#include "loaders/NPlane_Loader.h"
#include "loaders/WireFrame_Loader.h"
#include "loaders/Slice_Loader.h"


#include "src/Event.h"
#include "src/EventHandler.h"
#include "Exception.h"
//#include "Model.h"
#include "ModelHandler.h"
#include "ModelLoader.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"
//#include "ObjectLoader.h"
#include "Render.h"
//#include "StateLoader.h"
//#include "WorldEntity.h"


namespace Sear {

ModelHandler::ModelHandler() :
  _initialised(false)
	
{
  // TODO: this is not the place
  new Cal3d_Loader(this);
  new BoundBox_Loader(this);
  new WireFrame_Loader(this);
  new NPlane_Loader(this);
  new Slice_Loader(this);
  new ThreeDS_Loader(this);
}

ModelHandler::~ModelHandler() {
  if (_initialised) shutdown();
}

void ModelHandler::init() {
  if (_initialised) shutdown();
  // TODO: Do  clean up if required
//  _model_loaders = ModelLoaderMap();
  _model_records = ModelRecordMap();
  // TODO stop this
  System::instance()->getEventHandler()->addEvent(Event(EF_FREE_MODELS, NULL, EC_TIME, 60000 + System::instance()->getTime()));
  _initialised = true;
}

void ModelHandler::shutdown() {
  // Clean up model loaders
  for (ModelLoaderMap::iterator I = _model_loaders.begin(); I != _model_loaders.end(); ++I) {
    if (I->second) {
//      I->second->shutdown();
      delete I->second;
      _model_loaders[I->first] = NULL;
    }
  } 
// Clean Up Models
  for (ModelRecordMap::iterator I = _model_records.begin(); I != _model_records.end(); ++I) {
    if (I->second) {
      Model *model = I->second->model;
      if (model) {
        model->shutdown();
	delete model;
      }
      delete I->second;
      _model_records[I->first] = NULL;
    }
  }
  _initialised = false;
}
  
ModelRecord *ModelHandler::getModel(Render *render, ObjectRecord *record, const std::string &model_id) {
  // Model loaded for this object?
  if (_object_map[record->id + model_id]) return _object_map[record->id + model_id];
  
  if (_model_records[model_id]) {
    if (_model_records[model_id]);
    _object_map[record->id + model_id] = _model_records[model_id];
    return _model_records[model_id];
  }
  if (!render) {
    std::cerr << "renderer is null" << std::endl;	  
    return NULL;
  }
  if (!record) {
    std::cerr << "record is NULL" << std::endl;
    return NULL;
  }
  ModelRecord *model = NULL;
  varconf::Config *model_config = System::instance()->getModelRecords();
  std::string model_loader = (std::string)model_config->getItem(model_id, ModelRecord::MODEL_LOADER);
  
  if (_model_loaders[model_loader]) model = _model_loaders[model_loader]->loadModel(render, record, model_id, model_config);
  else {
    std::cerr << "No loader found: " << model_loader << std::endl;
    return NULL;
  }

  if (model->model_by_type)  _model_records[model_id] = model;
  _object_map[record->id + model_id] = model;
  return model; 
}

void ModelHandler::registerModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  // Check for bad values	
  if (model_type.empty()) throw Exception("No type specified");
  if (!model_loader) throw Exception("No model loader given");
  // Throw error if we already have a loader for this type
  // TODO: decide whether we should override existing loaders with new one.
  if (_model_loaders[model_type]) throw Exception("Model loader already exists for this type!");
  // If all is well, assign loader
  _model_loaders[model_type] = model_loader;
}

void ModelHandler::unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  // Only unregister a model laoder if it is properly registered
  if (_model_loaders[model_type] == model_loader) _model_loaders[model_type] = NULL;
}

void ModelHandler::checkModelTimeouts() {
  // TODO what about records with no model?
  std::cout << "Checking Timeouts" << std::endl;
  std::set<ModelRecord*> expired_set;
  for (ModelRecordMap::iterator I = _model_records.begin(); I != _model_records.end(); ++I) {
    ModelRecord *record = I->second;
    if (record) {
      Model *model = record->model;
      if (model) {
        if ((System::instance()->getTimef() - model->getLastTime()) > 60.0f) {
          expired_set.insert(record);
          _model_records[I->first] = NULL;
        }
      }
    }
  }
  for (ObjectRecordMap::iterator I = _object_map.begin(); I != _object_map.end(); ++I) {
    ModelRecord *record = I->second;
    if (record) {
      Model *model = record->model;
      if (model) {
        if ((System::instance()->getTimef() - model->getLastTime()) > 60.0f) {
          expired_set.insert(record);
          _object_map[I->first] = NULL;
        }
      }
    }
  }
  while (!expired_set.empty()) {
    ModelRecord *record = *expired_set.begin();
    if (record) {
      std::cout << "Unloading: " << record->id << std::endl;
      Model *model = record->model;
      if (model) delete model;
      delete record;
    }
    expired_set.erase(expired_set.begin());
  }
  System::instance()->getEventHandler()->addEvent(Event(EF_FREE_MODELS, NULL, EC_TIME, 60000 + System::instance()->getTime()));
}

void ModelHandler::checkModelTimeout(const std::string &id) {
//  Model *model = _models[id];
//  if (!model) return;
//  if (!model->getInUse()) {
//    if (model->getFlag("ModelByType")) return;
//    Log::writeLog(std::string("Unloading model for ") + id, Log::LOG_INFO);
//    model->shutdown();
//    delete model;
//    _models[id] = NULL;
//  }
}

} /* namespace Sear */
