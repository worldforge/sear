// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "System.h"
#include <set>
#include <string.h>

#include <varconf/Config.h>
#include <Eris/TypeInfo.h>

#include "common/Log.h"

#include "loaders/3ds_Loader.h"
#include "loaders/Cal3d_Loader.h"
#include "loaders/BoundBox_Loader.h"
#include "loaders/BillBoard_Loader.h"
#include "loaders/Impostor_Loader.h"
#include "loaders/NPlane_Loader.h"
#include "loaders/WireFrame_Loader.h"

#include "Exception.h"
#include "Model.h"
#include "ModelHandler.h"
#include "ModelLoader.h"
#include "ObjectLoader.h"
#include "Render.h"
#include "StateLoader.h"
#include "WorldEntity.h"


namespace Sear {

ModelHandler::ModelHandler() :
  detail_level(1.0f)
	
{
  // TODO: this is not the place
  new Cal3d_Loader(this);
  new BoundBox_Loader(this);
  new BillBoard_Loader(this);
  new WireFrame_Loader(this);
  new Impostor_Loader(this);
  new NPlane_Loader(this);
  new ThreeDS_Loader(this);
}

ModelHandler::~ModelHandler() {
  //TODO: Check that clean up has been performed
}

void ModelHandler::init() {
  // TODO: Do  clean up if required
  _model_loaders = std::map<std::string, ModelLoader*>();
  _models = std::map<std::string, Model*>();
}

void ModelHandler::shutdown() {
  // Clear up model loaders
  for (std::map<std::string, ModelLoader*>::iterator I = _model_loaders.begin(); I != _model_loaders.end(); I++ ) {
    if (I->second) {
      I->second->shutdown();
      delete I->second;
      _model_loaders[I->first] = NULL;
    }
  } 
 // Clear Up Model
  for (std::map<std::string, Model*>::iterator I = _models.begin(); I != _models.end(); I++) {
    if (I->second) {
      I->second->shutdown();
      delete I->second;
      _models[I->first] = NULL;
    }
  }
}
  
Model *ModelHandler::getModel(Render *render, WorldEntity *we) {
  // Check for NULL pointer	
  if (!we) throw Exception("WorldEntity is NULL");
  
  // Get model details
  // TODO: find out whether typeinfo will ever be more than two layers
  std::string type = we->type();
  std::string parent = we->parent();
  std::string id = we->getID();

  // If entity already has an associated model, return it.
  if (_models[id]) return _models[id];
///  if (_models[type]) return _models[type];
  if (!render) return NULL;
  
  // Get object type record
  ObjectProperties *op = we->getObjectProperties();
  ObjectLoader *ol = System::instance()->getObjectLoader();
  std::string object_type = "";
  std::string data_source = "";
  Model *model = NULL;
  ModelStruct *ms = (ModelStruct*)malloc(sizeof(ModelStruct));
  memset(ms, 0, sizeof(ModelStruct));
  ms->type = type.c_str();
  ms->parent = parent.c_str();
  static varconf::Config *model_config = System::instance()->getModel();

  object_type = we->type();

  std::set<std::string> parents_list = we->getType()->getParentsAsSet();
  std::set<std::string>::const_iterator I = parents_list.begin();
  while(!model) {
    op = ol->getObjectProperties(object_type);
    if (op) {
      data_source = model_config->getItem(op->model_type, object_type);
      ms->file_name = data_source.c_str();
      if (op->model_by_type) {
        if (_models[object_type]) {
          model = _models[object_type];
	  break;
        }
      }
      ms->width = op->width;
      ms->height = op->height;
      ms->wrap_texture = op->wrap_texture;
      ms->scale = op->scale;
      ms->num_planes = op->num_planes;
      ms->hasBBox = we->hasBBox();
      ms->bbox = we->getBBox();
      if (_model_loaders[op->model_type]) model = _model_loaders[op->model_type]->loadModel(render, *ms);
    }
    if (!model) {
      if (I == parents_list.end()) {
        if (object_type == "default") break;
	else object_type = "default";
      } else {
        object_type = *I;
        I++;
      }    
    }
  }
  if (!op) Log::writeLog("ERROR OP IS NULL", Log::LOG_ERROR);
  _models[id] = model;
  we->setObjectProperties(op);

  _models[id] = model;
  if (op->model_by_type) {
    model->setFlag("ModelByType", true);
    _models[object_type] = model;
  }
  if (!model) Log::writeLog(id, Log::LOG_ERROR);

  if (model) {
    model->setSelectState(System::instance()->getStateLoader()->getStateProperties(op->select_state));
  }
  free(ms);
  return model; 
}

Model *ModelHandler::getModel(Render *render, const std::string &object_type, ObjectProperties *op) {
  if (!render) return NULL;
  // Get object type record
//  System
  ObjectLoader *ol = System::instance()->getObjectLoader();
  Model *model = NULL;

  static varconf::Config *model_config = System::instance()->getModel();
  if(ol->getObjectProperties(object_type)) {
    memcpy(op,ol->getObjectProperties(object_type), sizeof(ObjectProperties));
  }
  if (op) {
    ModelStruct *ms = (ModelStruct*)malloc(sizeof(ModelStruct));
    memset(ms, 0, sizeof(ModelStruct));
    ms->type = object_type.c_str();
    ms->parent = NULL;
    ms->width = op->width;
    ms->height = op->height;
    ms->wrap_texture = op->wrap_texture;
    ms->scale = op->scale;
    ms->num_planes = op->num_planes;
    ms->hasBBox = false;
    ms->bbox = WFMath::AxisBox<3>();
    std::string data_source = model_config->getItem(op->model_type, object_type);
    ms->file_name = data_source.c_str();
    if (_model_loaders[op->model_type]) model = _model_loaders[op->model_type]->loadModel(render, *ms);
    free (ms);
  }
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
void ModelHandler::checkModelTimeout(const std::string &id) {
  Model *model = _models[id];
  if (!model) return;
  if (!model->getInUse()) {
    if (model->getFlag("ModelByType")) return;
    Log::writeLog(std::string("Unloading model for ") + id, Log::LOG_INFO);
    model->shutdown();
    delete model;
    _models[id] = NULL;
  }
}

void ModelHandler::lowerDetail() {
  detail_level -= 0.05f;
  if (detail_level < 0.0f) detail_level = 0.0f;
  for (std::map<std::string, Model*>::iterator I = _models.begin(); I != _models.end(); I++) {
    if (I->second) {
      I->second->setDetailLevel(detail_level);
    }
  }
}

void ModelHandler::raiseDetail() {
  detail_level += 0.05f;
  if (detail_level > 1.0f) detail_level = 1.0f;
  for (std::map<std::string, Model*>::iterator I = _models.begin(); I != _models.end(); I++) {
    if (I->second) {
      I->second->setDetailLevel(detail_level);
    }
  }
}

} /* namespace Sear */
