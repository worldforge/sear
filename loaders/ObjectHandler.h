// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

// $Id: ObjectHandler.h,v 1.9 2008-10-05 12:17:09 simon Exp $

#ifndef SEAR_LOADERS_OBJECTHANDLER_H
#define SEAR_LOADERS_OBJECTHANDLER_H 1

#include <map>
#include <string>

#include <sigc++/trackable.h>

#include "common/SPtr.h"

#include "interfaces/ConsoleObject.h"


namespace varconf {
  class Config;
}

namespace Sear {

class Console;
class ObjectRecord;
	
class ObjectHandler : public ConsoleObject, public sigc::trackable {
public:
  ObjectHandler();
  ~ObjectHandler();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  SPtr<ObjectRecord> getObjectRecord(const std::string &id);
  SPtr<ObjectRecord> instantiateRecord(const std::string &type, const std::string &id);

  void contextCreated() {}
  void contextDestroyed(bool check) {}

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
  void reset();

protected:
  void loadObjectRecords(const std::string &filename);
  typedef std::map<std::string, SPtr<ObjectRecord> > ObjectRecordMap;

  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  bool m_initialised;
  ObjectRecordMap m_type_map;
  ObjectRecordMap m_id_map;

  std::list<std::string> m_object_configs;
};
	
} /* namespace Sear */

#endif /* SEAR_LOADERS__OBJECTHANDLER_H */
