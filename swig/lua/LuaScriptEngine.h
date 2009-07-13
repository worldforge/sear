// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall

#ifndef SEAR_LUASCRIPTENGINE_H
#define SEAR_LUASCRIPTENGINE_H 1

#include "interfaces/ConsoleObject.h"

// include Lua libs
extern "C" {
 #include "lua.h"
 #include "lualib.h"
 #include "lauxlib.h"
}

namespace Sear {

class Console;

class LuaScriptEngine : public ConsoleObject {

public:
  LuaScriptEngine();
  ~LuaScriptEngine();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  static int stackDump(lua_State *L);
  static void printLuaError(int error);

private:  
  bool m_initialised;
  lua_State *m_state;   
};

} /* namespace Sear */

#endif /* SEAR_LUASCRIPTENGINE_H */
