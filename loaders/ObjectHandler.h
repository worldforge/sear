// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ObjectHandler.h,v 1.1 2005-01-06 12:46:54 simon Exp $

#ifndef SEAR_OBJECTHANDLER_H
#define SEAR_OBJECTHANDLER_H 1

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

  void loadObjectRecords(const std::string &id);
  ObjectRecord *getObjectRecord(const std::string &id);
  void copyObjectRecord(const std::string &id, ObjectRecord *);

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
protected:
  bool _initialised;
  typedef std::map<std::string, ObjectRecord*> ObjectRecordMap;
  ObjectRecordMap _object_records;

  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);
  
};
	
} /* namespace Sear */

#endif /* SEAR_OBJECTHANDLER_H */
