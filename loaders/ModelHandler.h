// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: ModelHandler.h,v 1.3 2005-03-15 17:55:03 simon Exp $

#ifndef SEAR_LOADERS_MODELHANDLER_H
#define SEAR_LOADERS_MODELHANDLER_H 1

#include <map>
#include <string>

#include <Eris/Timeout.h>

#include "interfaces/ConsoleObject.h"

namespace varconf  {
  class Config;
}

namespace Sear {

// Forward Declarationa
class Console;
class ModelLoader;
class ModelRecord;
class ObjectRecord;
class Render;
class WorldEntity;
	
class ModelHandler : public SigC::Object , public ConsoleObject {
public:
  ModelHandler();
  ~ModelHandler();

  void init();
  void shutdown();
 
  void invalidate() {};
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
 
  ModelRecord *getModel(Render *render, ObjectRecord *record, const std::string &model_id, WorldEntity *we = NULL);

  void registerModelLoader(const std::string &model_type, ModelLoader *model_loader);
  void unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader);

  void checkModelTimeouts();
  
  void loadModelRecords(const std::string &filename);

  varconf::Config &getModelRecords() { return m_model_records; }

protected:
  void TimeoutExpired();
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  typedef std::map<std::string, ModelLoader*> ModelLoaderMap;
  typedef std::map<std::string, ModelRecord*> ModelRecordMap; 
  typedef std::map<std::string, ModelRecord*> ObjectRecordMap;

  ModelLoaderMap m_model_loaders; // Stores all the model loaders
  ModelRecordMap m_model_records_map; // Stores all the model_by_type models
  ObjectRecordMap m_object_map; // Stores model for entity id and model_name
  bool m_initialised;
  Eris::Timeout *m_timeout;
  varconf::Config m_model_records;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_MODELHANDLER_H */
