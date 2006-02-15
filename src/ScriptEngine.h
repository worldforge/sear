// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ScriptEngine.h,v 1.5 2006-02-15 12:44:24 simon Exp $

#ifndef SEAR_SCRIPTENGINE_H
#define SEAR_SCRIPTENGINE_H 1

#include "interfaces/ConsoleObject.h"

namespace Sear {

class Console;

class ScriptEngine : public ConsoleObject {

public:
  ScriptEngine();
  ~ScriptEngine();

  void init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }
 
  void runScript(const std::string &file_name);
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  std::string getFileDirectory() const { return m_file_dir; }
  
private:  
  bool m_initialised;
  std::string m_file_dir;
};

} /* namespace Sear */

#endif /* SEAR_SCRIPTENGINE_H */
