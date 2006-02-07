// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: ModelHandler.cpp,v 1.27 2006-02-07 11:31:03 simon Exp $

#include <set>
#include <string.h>

#include <Atlas/Message/Element.h>

#include <varconf/Config.h>
#include "src/Console.h"
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
#include "NullModel.h"

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

static const std::string ATTR_GUISE= "guise";
static const std::string ATTR_MODE = "mode";

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

  m_timeout = new Eris::Timeout(60000);
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
  m_object_map.clear();
  // Delete all remaining records
  m_model_records_map.clear();

  m_initialised = false;
}
  
SPtr<ModelRecord> ModelHandler::getModel(Render *render, const std::string &model_id, WorldEntity *we) {
  assert (m_initialised == true);

  assert(we);
  // Model loaded for this object?

  std::string id = we->getId() + model_id;
  ObjectRecordMap::const_iterator I = m_object_map.find(id);
  if (I != m_object_map.end()) {
    return I->second;
  }

  ModelRecordMap::const_iterator J = m_model_records_map.find(model_id);
  if (J != m_model_records_map.end()) {
    m_object_map[id] = J->second;
    return J->second;
  }
  // No existing model found, load up a new one
  if (!render) {
    if (debug && we) printf("Entity ID: %s Name: %s Type: %s\n", we->getId().c_str(), we->getName().c_str(), we->type().c_str());
    return SPtr<ModelRecord>();
  }

  varconf::Config &m_model_records = ModelSystem::getInstance().getModelRecords();
  std::string model_loader = (std::string)m_model_records.getItem(model_id, ModelRecord::MODEL_LOADER);

  if (model_loader.empty()) {
    printf("Model Loader not defined. Using BoundBox.\n");
    model_loader = "boundbox";
  }
  ModelLoaderMap::const_iterator K = m_model_loaders.find(model_loader);
  ModelLoaderMap::const_iterator Kend = m_model_loaders.end();
  if (K == Kend) {
    printf("Unknown Model Loader. Using BoundBox.\n");
    model_loader = "boundbox";
    K = m_model_loaders.find(model_loader);
  }
  SPtr<ModelRecord> model;
  if (K != Kend) {
    model = K->second->loadModel(render, we, model_id, m_model_records);
  } else {
    std::cerr << "No loader found: " << model_loader << std::endl;
    return model;
  }
  
  // Check model was loaded, and fall back to a NullModel
  if (!model) {
    std::cerr << "Error loading model of type " << model_loader << std::endl;
    model = SPtr<ModelRecord>(new ModelRecord);
    model->model = SPtrShutdown<Model>(new NullModel(render));
  }

//  if (we != NULL) {
    if (we->hasAttr(ATTR_MODE)) {
      model->model->animate(we->valueOfAttr(ATTR_MODE).asString());
    }
    if (we->hasAttr(ATTR_GUISE)) {
      Atlas::Message::MapType mt = we->valueOfAttr(ATTR_GUISE).asMap();
      model->model->setAppearance(mt);
    }                                                                          
//  }

  // If model is a generic one, add it to the generic list
  if (model->model_by_type) m_model_records_map[model_id] = model;
  m_object_map[id] = model;

  return model; 
}

void ModelHandler::registerModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  assert (m_initialised == true);
  assert(model_type.empty() == false);
  assert(model_loader != NULL);

  // Throw error if we already have a loader for this type
  assert(m_model_loaders[model_type] ==  NULL);

  // If all is well, assign loader
  m_model_loaders[model_type] = model_loader;
}

void ModelHandler::unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  assert (m_initialised == true);
  // Only unregister a model laoder if it is properly registered
  ModelLoaderMap::iterator I = m_model_loaders.find(model_type);
  if (I != m_model_loaders.end() && I->second == model_loader) {
    m_model_loaders.erase(I);
  }
}

void ModelHandler::checkModelTimeouts(bool forceUnload) {
  assert (m_initialised == true);

  // This function checks to see when the last time a model record was rendered.
  // If the time has been longer than a threshold, we unload the model record 
  // and associated models.

  // NOTE: Perhaps we could just unload the model and not the record?

  // Only look thorugh unique records list. 

  // TODO what about records with no model?
  if (debug) std::cout << "Checking Timeouts" << std::endl;

  std::set<std::string> expired_set;

  // Do the same again for the object map
  ModelRecordMap::iterator Jend = m_object_map.end();
  ObjectRecordMap::iterator J = m_object_map.begin();
  while (J != Jend) {
    SPtr<ModelRecord> record = J->second;
    SPtrShutdown<Model> model = record->model;
    bool unload = forceUnload;
    if (!unload && model) {
      if (System::instance()->getTimef() - model->getLastTime() > 60.0f) {
        unload = true;
      }
    }
    if (unload) m_object_map.erase(J++);
    else ++J;
  }

  // Loop through and find all objects that have expired and add to set object
  ModelRecordMap::iterator Iend = m_model_records_map.end();
  ModelRecordMap::iterator I = m_model_records_map.begin();
  while (I != Iend) {
    SPtr<ModelRecord> record = I->second;
    SPtrShutdown<Model> model = record->model;
    bool unload = forceUnload;
    if (!unload && model) {
      if (System::instance()->getTimef() - model->getLastTime() > 60.0f) {
        unload = true;
      }
    }
    if (unload) m_model_records_map.erase(I++);
    else ++I;
  }
}

void ModelHandler::TimeoutExpired() {
  checkModelTimeouts(false);
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
void ModelHandler::contextCreated() {
  for (ModelRecordMap::iterator I = m_model_records_map.begin(); I != m_model_records_map.end(); ++I) {
    SPtr<ModelRecord> record = I->second;

    SPtrShutdown<Model> model = record->model;
    if (model) {
      model->contextCreated();
    }
  }
}


void ModelHandler::contextDestroyed(bool check) {
  for (ModelRecordMap::iterator I = m_model_records_map.begin(); I != m_model_records_map.end(); ++I) {
    SPtr<ModelRecord> record = I->second;

    SPtrShutdown<Model> model = record->model;
    if (model) {
      model->contextDestroyed(check);
    }
  }
}

void ModelHandler::reset() {
  checkModelTimeouts(true);
}


PosAndOrient Model::getPositionForSubmodel(const std::string& submodelName)
{
  PosAndOrient po;
  po.pos = WFMath::Vector<3>(0.0f,0.0f,0.0f);
  po.orient = WFMath::Quaternion(1.0f, 0.0f,0.0f,0.0f);
//    std::cerr << "called getPositionForSubmodel on Model base class : undefined" << std::endl;
    return po;
}


} /* namespace Sear */
