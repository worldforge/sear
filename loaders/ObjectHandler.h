// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: ObjectHandler.h,v 1.2 2005-03-15 17:55:03 simon Exp $

#ifndef SEAR_LOADERS_OBJECTHANDLER_H
#define SEAR_LOADERS_OBJECTHANDLER_H 1

#include <map>
#include <string>

#include <sigc++/object_slot.h>

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
  ObjectRecord *getObjectRecord(const std::string &id);
  void copyObjectRecord(const std::string &id, ObjectRecord *);

  void invalidate() {}

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
   varconf::Config &getObjectRecords() { return m_object_records; }

protected:
  typedef std::map<std::string, ObjectRecord*> ObjectRecordMap;

  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  bool m_initialised;
  ObjectRecordMap m_object_records_map;
  varconf::Config m_object_records;
};
	
} /* namespace Sear */

#endif /* SEAR_LOADERS__OBJECTHANDLER_H */
