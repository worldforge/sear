// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ObjectHandler.h,v 1.4 2006-02-07 11:31:03 simon Exp $

#ifndef SEAR_LOADERS_OBJECTHANDLER_H
#define SEAR_LOADERS_OBJECTHANDLER_H 1

#include <map>
#include <string>

#include <sigc++/object_slot.h>

#include "common/SPtr.h"

#include "interfaces/ConsoleObject.h"


namespace varconf {
  class Config;
}

namespace Sear {

class Console;
class ObjectRecord;
	
class ObjectHandler : public ConsoleObject, public SigC::Object {
public:
  ObjectHandler();
  ~ObjectHandler();

  void init();
  void shutdown();

  void loadObjectRecords(const std::string &filename);
  SPtr<ObjectRecord> getObjectRecord(const std::string &id);
  SPtr<ObjectRecord> instantiateRecord(const std::string &type, const std::string &id);

  void contextCreated() {}
  void contextDestroyed(bool check) {}

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
  varconf::Config &getObjectRecords() { return m_object_records; }

  void reset();

protected:
  typedef std::map<std::string, SPtr<ObjectRecord> > ObjectRecordMap;

  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  bool m_initialised;
  ObjectRecordMap m_type_map;
  ObjectRecordMap m_id_map;
  varconf::Config m_object_records;
};
	
} /* namespace Sear */

#endif /* SEAR_LOADERS__OBJECTHANDLER_H */
