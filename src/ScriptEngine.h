// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ScriptEngine.h,v 1.2 2003-03-06 23:50:38 simon Exp $

#ifndef SEAR_SCRIPTENGINE_H
#define SEAR_SCRIPTENGINE_H 1

#include "src/ConsoleObject.h"

namespace Sear {

class Console;

class ScriptEngine : public ConsoleObject {

public:
  ScriptEngine();
  ~ScriptEngine();

  void init();
  void shutdown();
 
  void runScript(const std::string &file_name);
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  std::string getFileDirectory() const { return _file_dir; }
  bool prefixEnabled() const { return _prefix_enabled; }
  
  
private:  
   bool _initialised;

  bool _prefix_enabled;
  std::string _file_dir;
  
  
};

} /* namespace Sear */

#endif /* SEAR_SCRIPTENGINE_H */
