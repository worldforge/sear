// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ModelHandler.cpp,v 1.28 2002-11-12 23:59:22 simon Exp $

//#include "config.h"

#include "System.h"
#include <set>
#include <string.h>

#include <varconf/Config.h>


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
#include "ModelHandler.h"
#include "ModelLoader.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"
#include "Render.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

ModelHandler::ModelHandler() :
  _initialised(false)
	
{
  // TODO: this is not the place
  // create all the model loaders
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
  // TODO another method would be better - or a clean up of the event system
  System::instance()->getEventHandler()->addEvent(Event(EF_FREE_MODELS, NULL, EC_TIME, 60000 + System::instance()->getTime()));
  _initialised = true;
}

void ModelHandler::shutdown() {
  // Clean up model loaders
  while (!_model_loaders.empty()) {
    ModelLoader *ml = _model_loaders.begin()->second;
    if (ml) delete (ml);
    _model_loaders.erase(_model_loaders.begin());
  }
  // Delete all unique records
  while (!_object_map.empty()) {
    ModelRecord *mr = _object_map.begin()->second;
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
    _object_map.erase(_object_map.begin());
  }
  // Delete all remaining records
  while (!_model_records.empty()) {
    ModelRecord *mr = _model_records.begin()->second;
    if (mr) {
      if (mr->model) {
	mr->model->shutdown();
        delete mr->model;
        mr->model = NULL;
      }
      delete mr;
    }
    _model_records.erase(_model_records.begin());
  }
  _initialised = false;
}
  
ModelRecord *ModelHandler::getModel(Render *render, ObjectRecord *record, const std::string &model_id) {
  // Model loaded for this object?
  if (_object_map[record->id + model_id]) return _object_map[record->id + model_id];
  
  if (_model_records[model_id]) {
    _object_map[record->id + model_id] = _model_records[model_id];
    return _model_records[model_id];
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
  
  if (_model_loaders[model_loader]) model = _model_loaders[model_loader]->loadModel(render, record, model_id, model_config);
  else {
    std::cerr << "No loader found: " << model_loader << std::endl;
    return NULL;
  }
  // If model is a generic one, add it to the generic list
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
  // TODO throw execption is not a good idea as this method gets called via a constructor
  //if (_model_loaders[model_type]) throw Exception("Model loader already exists for this type!");
  // If all is well, assign loader
  _model_loaders[model_type] = model_loader;
}

void ModelHandler::unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  // Only unregister a model laoder if it is properly registered
  if (_model_loaders[model_type] == model_loader) _model_loaders[model_type] = NULL;
}

void ModelHandler::checkModelTimeouts() {
  // TODO what about records with no model?
  if (debug) std::cout << "Checking Timeouts" << std::endl;
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
