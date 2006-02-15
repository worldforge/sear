// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

#ifndef SEAR_LOADERS_MODELSYSTEM_H
#define SEAR_LOADERS_MODELSYSTEM_H 1

#include <string>

#include <sigc++/trackable.h>

#include <varconf/Config.h>

#include "interfaces/ConsoleObject.h"
#include "common/SPtr.h"

namespace Sear {

class Console;
class ModelHandler;
class ObjectHandler;
class WorldEntity;
class ObjectRecord;
class ModelRecord;

class ModelSystem : public sigc::trackable, public ConsoleObject {
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
 
  /** The getModel method returns a model record for the given object and model
   * ID. NULL is returned if no record can be found.
   * @param render The current render object. This is required to create the 
   * model instance.
   * @param model_id The name of the model record to instantiate
   * @param we The Entity owning this model.
   */
  SPtr<ModelRecord> getModel(const std::string &model_id, WorldEntity *we); 


  /** The getObjectRecord method searches for the most appropriate record
   * for the given entity. This will search through the type hierarchy if
   * required until a matching record is found. An instance of the record is
   * then associated with this entity.
   * @param we The Entity to get the ObjectRecord of.
   * @return The suitable ObjectRecord, or NULL if none is found.
   */
  SPtr<ObjectRecord> getObjectRecord(WorldEntity *we);


  /** The resetModels method removes all entity based information stored.
   */
  void resetModels();

private:

  void loadModels(const std::string &filename);

  static ModelSystem m_instance;

  bool m_initialised;

  varconf::Config m_models;

  ModelHandler  *m_model_handler;
  ObjectHandler *m_object_handler;

};

} // namespace Sear

#endif /* SEAR_LOADERS_MODELSYSTEM_H */
