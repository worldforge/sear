// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: ModelHandler.h,v 1.10 2006-04-30 18:13:41 alriddoch Exp $

#ifndef SEAR_LOADERS_MODELHANDLER_H
#define SEAR_LOADERS_MODELHANDLER_H 1

#include <map>
#include <string>

#include <varconf/Config.h>

#include <sigc++/trackable.h>

#include "interfaces/ConsoleObject.h"

#include "common/SPtr.h"

namespace varconf  {
  class Config;
}

namespace Eris {
  class Timeout;
}

namespace Sear {

// Forward Declarationa
class Console;
class ModelLoader;
class ModelRecord;
class ObjectRecord;
class WorldEntity;
	
class ModelHandler : public sigc::trackable , public ConsoleObject {
public:
  ModelHandler();
  ~ModelHandler();

  void init();
  void shutdown();
 
  void contextCreated();
  void contextDestroyed(bool check);
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
 
  SPtr<ModelRecord> getModel(const std::string &model_id, WorldEntity *we = NULL);

  void registerModelLoader(SPtr<ModelLoader> model_loader);
  void unregisterModelLoader(const std::string &model_type);

  void checkModelTimeouts(bool forceUnload);
  
  void loadModelRecords(const std::string &filename);

  varconf::Config &getModelRecords() { return m_model_records; }

  void reset();

protected:
  void TimeoutExpired();
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  typedef std::map<std::string, SPtr<ModelLoader> > ModelLoaderMap;
  typedef std::map<std::string, SPtr<ModelRecord> > ModelRecordMap; 
  typedef std::map<std::string, SPtr<ModelRecord> > ObjectRecordMap;

  ModelLoaderMap m_model_loaders; // Stores all the model loaders
  ModelRecordMap m_model_records_map; // Stores all the model_by_type models
  ObjectRecordMap m_object_map; // Stores model for entity id and model_name
  bool m_initialised;
  Eris::Timeout *m_timeout;
  varconf::Config m_model_records;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_MODELHANDLER_H */
