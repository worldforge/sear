// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ScriptEngine.h,v 1.1 2002-10-21 20:09:59 simon Exp $

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
  static const char * const CHANGE_DIRECTORY = "cd";
  static const char * const CHANGE_TO_FILE_DIRECTORY = "cd_this_dir";
  static const char * const ENABLE_DIR_PREFIX = "enable_dir_prefix";
  static const char * const DISABLE_DIR_PREFIX = "disable_dir_prefix";
  static const char * const RUN_SCRIPT = "run_script";
  static const char * const SEARCH_RUN_SCRIPT = "search_run_script";
 
  bool _initialised;

  bool _prefix_enabled;
  std::string _file_dir;
  
  
};

} /* namespace Sear */

#endif /* SEAR_SCRIPTENGINE_H */
