// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2007 Simon Goodall

#include <sigc++/object_slot.h>

#include <Eris/TypeInfo.h>

#include "renderers/RenderSystem.h"

#include "src/Console.h"
#include "src/System.h"
#include "src/WorldEntity.h"

#include "EntityMapper.h"
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
#include "SearObject_Loader.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static censt bool debug = false;
#endif

namespace Sear {

ModelSystem ModelSystem::m_instance;

static const std::string DEFAULT = "default";

int ModelSystem::init() {
  assert(m_initialised == false);

  m_model_handler = SPtrShutdown<ModelHandler>(new ModelHandler());
  m_model_handler->init();

  // Register ModelLoaders
  // The smart pointer makes sure they get clean up.
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new BoundBox_Loader())); // This is the default loader
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new WireFrame_Loader()));
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new NPlane_Loader()));
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new SearObject_Loader()));
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new ThreeDS_Loader()));
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new LibModelFile_Loader()));
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new AreaModelLoader()));
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new ParticleSystemLoader()));
  // The cal3d loader requires the particle sys loader otherwise the
  // does not get linked in correctly DynamicObject
  m_model_handler->registerModelLoader(SPtr<ModelLoader>(new Cal3d_Loader()));
  
  m_object_handler = SPtrShutdown<ObjectHandler>(new ObjectHandler());
  m_object_handler->init();

  m_entity_mapper = SPtrShutdown<EntityMapper>(new EntityMapper());
  m_entity_mapper->init();

  RenderSystem::getInstance().ContextCreated.connect(sigc::mem_fun(this, &ModelSystem::contextCreated));
  RenderSystem::getInstance().ContextDestroyed.connect(sigc::mem_fun(this, &ModelSystem::contextDestroyed));

  System::instance()->LeftWorld.connect(sigc::mem_fun(this, &ModelSystem::resetModels));

  m_initialised = true;
  return 0;
}

int ModelSystem::shutdown() {
  assert(m_initialised == true);

  m_object_handler.release();

  m_model_handler.release();

  m_entity_mapper.release();

  // Cleanp signals
  notify_callbacks();

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

  m_model_handler->registerCommands(console);
  m_object_handler->registerCommands(console);
  m_entity_mapper->registerCommands(console);
}

void ModelSystem::runCommand(const std::string &command, const std::string &args) {
}

void ModelSystem::readConfig(varconf::Config &config) {

}

void ModelSystem::writeConfig(varconf::Config &config) {

}

SPtr<ModelRecord> ModelSystem::getModel(const std::string &model_id, WorldEntity *we) {
  return m_model_handler->getModel(model_id, we);
}

SPtr<ObjectRecord> ModelSystem::getObjectRecord(WorldEntity *we) {
  assert (we != NULL);
  // Find object record from entity id
  const std::string &id = we->getId();
  SPtr<ObjectRecord> object_record = m_object_handler->getObjectRecord(id);

  if (!object_record) {
    const std::string &mapper = m_entity_mapper->getEntityMapping(we);
    if (mapper != "") {
      object_record = m_object_handler->instantiateRecord(mapper, id);
      if (object_record) {
        object_record->type = we->type();
        object_record->name = we->getName();
        object_record->id = we->getId();
        object_record->entity = we;
      }
    }
  }
  if (!object_record) {
    std::string type = DEFAULT;
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
          // No match in the type hierarchy so request default record.
          object_record = m_object_handler->instantiateRecord("default", id);
          assert(object_record);
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
