// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

#include <sigc++/object_slot.h>

#include <Eris/TypeInfo.h>

#include "renderers/RenderSystem.h"

#include "src/Console.h"
#include "src/System.h"
#include "src/WorldEntity.h"

#include "ModelSystem.h"
#include "ObjectHandler.h"
#include "ModelHandler.h"
#include "ObjectRecord.h"
#include "ModelRecord.h"
#include "Model.h"

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

  RenderSystem::getInstance().ContextCreated.connect(SigC::slot(*this, &ModelSystem::contextCreated));
  RenderSystem::getInstance().ContextDestroyed.connect(SigC::slot(*this, &ModelSystem::contextDestroyed));

  System::instance()->LeftWorld.connect(SigC::slot(*this, &ModelSystem::resetModels));

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

void ModelSystem::contextCreated() {
  assert(m_initialised);

  m_model_handler->contextCreated();
  m_object_handler->contextCreated();
}

void ModelSystem::contextDestroyed(bool check) {
  assert(m_initialised);

  m_model_handler->contextDestroyed(check);
  m_object_handler->contextDestroyed(check);
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
  if (debug) printf("ModelSystem: Loading Models config.\n");
  m_models.readFromFile(filename);
}

SPtr<ModelRecord> ModelSystem::getModel(Render *render, const std::string &model_id, WorldEntity *we) {
  return m_model_handler->getModel(render, model_id, we);
}

SPtr<ObjectRecord> ModelSystem::getObjectRecord(WorldEntity *we) {
  assert (we != NULL);
  // Find object record from entity id
  std::string id = we->getId();
  SPtr<ObjectRecord> object_record = m_object_handler->getObjectRecord(id);

  std::string type = DEFAULT;
  if (!object_record) {
    Eris::TypeInfo *ti = we->getType();

    while (ti != NULL) {
      std::string t = ti->getName();
      varconf::Config::inst()->clean(t);
      object_record = m_object_handler->instantiateRecord(t, id);
      if (object_record) {
        type = t;
        break;
      } else {
        // TODO, Check this output
        // I.e, will there be more than one?
        // And if so, will it be the same as getParent on each type too?
        if (ti->getParents().size() > 0) {
          ti = *(ti->getParents().begin());
        } else {
          ti = NULL;
        }
      }
    }

    // Set the values for the object record
    object_record->type = type;
    object_record->name = we->getName();
    object_record->id = we->getId();
    object_record->entity = we;
  }

  assert(object_record->entity.get() != NULL);

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

void ModelSystem::resetModels() {
  m_object_handler->reset();
  m_model_handler->reset();
}

} // namespace Sear
