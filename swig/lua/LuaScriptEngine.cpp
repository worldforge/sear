// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#include "LuaScriptEngine.h"

// include Lua libs and tolua++
extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"

  extern int luaopen_Guichan(lua_State* L); // declare the wrapped module
  extern int luaopen_Sear(lua_State* L); // declare the wrapped module
  extern int luaopen_Eris(lua_State* L); // declare the wrapped module
}

#include "src/Console.h"


namespace Sear {

void LuaScriptEngine::printLuaError(int error) {
  std::string message;
  switch (error) {
    case (LUA_ERRRUN): message = "Error while running chunk"; break;
    case (LUA_ERRSYNTAX): message = "Syntax error occured during pre-compilation"; break;
    case (LUA_ERRMEM): message = "Memory allocation error"; break;
    case (LUA_ERRFILE): message = "Error opening file"; break;
    case (LUA_ERRERR): message = "Error Error"; break;
    default: message = "Unknown error"; break;
  }
  printf("LUA ERROR: %s\n", message.c_str());
}


int LuaScriptEngine::stackDump (lua_State *L) {
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++) {  /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {
      case LUA_TSTRING:  /* strings */
        printf("`%s'", lua_tostring(L, i));
        break;
    
      case LUA_TBOOLEAN:  /* booleans */
        printf(lua_toboolean(L, i) ? "true" : "false");
        break;
    
      case LUA_TNUMBER:  /* numbers */
        printf("%g", lua_tonumber(L, i));
        break;
    
      default:  /* other values */
        printf("%s", lua_typename(L, t));
        break;
    
    }
    printf("  ");  /* put a separator */
  }
  printf("\n");  /* end the listing */

  return 1;
}


LuaScriptEngine::LuaScriptEngine()
{
}


LuaScriptEngine::~LuaScriptEngine()
{
	printf("Shutting down lua environment.\n");
	lua_close(m_state);
}


int LuaScriptEngine::init() {

  m_state = lua_open();

  // load the default libs
  luaL_openlibs(m_state);

  // load in custom handlers
  luaopen_Guichan(m_state);
  luaopen_Eris(m_state);
  luaopen_Sear(m_state);

  return 0;
}

void LuaScriptEngine::shutdown() {
}


void LuaScriptEngine::registerCommands(Console *console) {
  console->registerCommand("lua", this);
}

void LuaScriptEngine::runCommand(const std::string &command, const std::string &args) {
  if (command == "lua") {
    // Store current stack pos
    int oldtop = lua_gettop(m_state);

   int error = luaL_dostring(m_state, args.c_str());
   if (error != 0) {
      printf( "error running function `f': %s\n",
                 lua_tostring(m_state, -1));
      stackDump(m_state);  

      printLuaError(error); // Print error messages
    }
    // Restore stack top
    lua_settop(m_state, oldtop);
  }
}


/*
void ScriptEngine::runCommand(const std::string &command) {
  if (command.empty()) return;
  if (debug) std::cout << "Running: " << command << std::endl;
  // Ignores any returned values
  int oldtop = lua_gettop(m_state); // Store stack top
  int error = lua_dostring(m_state, command.c_str());
  if (error != 0) printError(error); // Print error messages
  lua_settop(m_state, oldtop); // Restore stack top
}
*/
} // namespace Sear
