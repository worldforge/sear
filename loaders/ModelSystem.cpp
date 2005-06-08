// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Simon Goodall

#include "src/Console.h"
#include "src/WorldEntity.h"

#include "ModelSystem.h"
#include "ObjectHandler.h"
#include "ModelHandler.h"
#include "ObjectRecord.h"

#include "3ds_Loader.h"
#include "cal3d/Cal3d_Loader.h"
#include "BoundBox_Loader.h"
#include "NPlane_Loader.h"
#include "WireFrame_Loader.h"
#include "LibModelFile_Loader.h"
#include "AreaModelLoader.h"
#include "ParticleSystemLoader.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

ModelSystem ModelSystem::m_instance;

static const std::string CMD_LOAD_MODEL_CONFIG = "load_models";
static const std::string DEFAULT = "default";

int ModelSystem::init() {
  assert(m_initialised == false);

  m_model_handler = new ModelHandler();
  m_model_handler->init();

  // Register ModelLoaders
  // The ModelHandler class cleans these up.
  new BoundBox_Loader(m_model_handler); // This is the default loader
  new WireFrame_Loader(m_model_handler);
  new NPlane_Loader(m_model_handler);
  new ThreeDS_Loader(m_model_handler);
  new Cal3d_Loader(m_model_handler);
  new LibModelFile_Loader(m_model_handler);
  new AreaModelLoader(m_model_handler);
  new ParticleSystemLoader(m_model_handler);
  
  m_object_handler = new ObjectHandler();
  m_object_handler->init();

  m_initialised = true;
  return 0;
}

int ModelSystem::shutdown() {
  assert(m_initialised == true);

  m_object_handler->shutdown();
  delete m_object_handler;
  m_object_handler = NULL;

  m_model_handler->shutdown();
  delete m_model_handler;
  m_model_handler = NULL;

  m_initialised = false;
  return 0;
}

void ModelSystem::invalidate() {
  assert(m_initialised);

  m_model_handler->invalidate();
  m_object_handler->invalidate();
}

void ModelSystem::registerCommands(Console *console) {
  assert(m_initialised == true);
  assert(console != NULL);

  console->registerCommand(CMD_LOAD_MODEL_CONFIG, this);
  
  m_model_handler->registerCommands(console);
  m_object_handler->registerCommands(console);
}

void ModelSystem::runCommand(const std::string &command, const std::string &args) {
  if (command == CMD_LOAD_MODEL_CONFIG) {
    loadModels(args);
  }
}

void ModelSystem::readConfig(varconf::Config &config) {

}

void ModelSystem::writeConfig(varconf::Config &config) {

}

void ModelSystem::loadModels(const std::string &filename) {
  if (debug) printf("ModelSystem: Loading Models config\n");
  m_models.readFromFile(filename);
}

ModelRecord *ModelSystem::getModel(Render *render, ObjectRecord *record, const std::string &model_id, WorldEntity *we) {
  return m_model_handler->getModel(render, record, model_id, we);
}

ObjectRecord *ModelSystem::getObjectRecord(WorldEntity *we) {
  // Find object record from entity id
  ObjectRecord *object_record = m_object_handler->getObjectRecord(we->getId());

  // Why did we do this? 
//  if (object_record && object_record->type.empty()) {
//    object_record->type = we->getId();
//  }

  if (!object_record) {
    // No record for this ID yet, so lets see if there is a record for 
    // the object type
    std::string type = we->type();
    varconf::Config::inst()->clean(type);
    object_record = m_object_handler->getObjectRecord(type);

    if (object_record == NULL) {
      // No record for object type either! Lets try the parent type then
      type = we->parent();
      varconf::Config::inst()->clean(type);
      object_record = m_object_handler->getObjectRecord(type);
    }

    if (object_record == NULL) {
      // Still no record? then lets use the default record
      type = DEFAULT;
      varconf::Config::inst()->clean(type);
      object_record = m_object_handler->getObjectRecord(type);
      if (debug) printf("Object ID: %s Type: %s using default record.\n", we->getId().c_str(), we->type().c_str());
    }
 
    assert(object_record);
    // Copy existing record to the entity ID so we can make entity
    // Specific changes
    m_object_handler->copyObjectRecord(we->getId(), object_record);
    // Get pointer to our new record
    object_record = m_object_handler->getObjectRecord(we->getId());

    assert(object_record);

    // Set the values for the object record
    object_record->type = type;
    object_record->name = we->getName();
    object_record->id = we->getId();
    object_record->entity = we;
  }

  // Copy bounding box value
  if (we->hasBBox()) {
    object_record->bbox = we->getBBox();
  }

  object_record->position = we->getAbsPos();
  object_record->orient = we->getAbsOrient();

  assert (object_record->type.empty() == false);

  return object_record;
}

varconf::Config &ModelSystem::getModelRecords() {
  return m_model_handler->getModelRecords();
}

//varconf::Config &ModelSystem::getObjectRecords() {
//  return m_object_handler->getObjectRecords();
//}

} // namespace Sear
