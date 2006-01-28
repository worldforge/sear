// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Simon Goodall

#ifndef SEAR_LOADERS_MODELSYSTEM_H
#define SEAR_LOADERS_MODELSYSTEM_H 1

#include <string>

#include <sigc++/object_slot.h>

#include <varconf/Config.h>

#include "interfaces/ConsoleObject.h"

namespace Sear {

class Render;
class Console;
class ModelHandler;
class ObjectHandler;
class WorldEntity;
class ObjectRecord;
class ModelRecord;

class ModelSystem : public SigC::Object, public ConsoleObject {
public:

  static ModelSystem &getInstance() { return m_instance; }

  ModelSystem() :
    m_initialised(false)
  { }
  virtual ~ModelSystem() {}

  int init();
  int shutdown();

  void contextCreated();
  void contextDestroyed(bool check);

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  ModelHandler  *getModelHandler()  const { return m_model_handler; }
  ObjectHandler *getObjectHandler() const { return m_object_handler; }

  varconf::Config &getModels() { return m_models; }
  varconf::Config &getModelRecords();
//  varconf::Config &getObjectRecords();
//  varconf::Config &getObjectRecords() { return m_object_records; }
 
  ModelRecord *getModel(Render *render, ObjectRecord *record, const std::string &mode_id, WorldEntity *we); 

  ObjectRecord *getObjectRecord(WorldEntity *we);

  void resetModels();

private:

  void loadModels(const std::string &filename);

  static ModelSystem m_instance;

  bool m_initialised;

  varconf::Config m_models;
//  varconf::Config m_model_records;
//  varconf::Config m_object_records;

  ModelHandler  *m_model_handler;
  ObjectHandler *m_object_handler;

};

} // namespace Sear

#endif /* SEAR_LOADERS_MODELSYSTEM_H */
