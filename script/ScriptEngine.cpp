// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ScriptEngine.cpp,v 1.1.2.2 2003-01-05 18:18:28 simon Exp $

#include "ScriptEngine.h"

#include <iostream>

extern "C" {
  #include <lualib.h>
}

#include "SEFunctions.h"

namespace Sear {

static const bool debug = true;	
	
ScriptEngine *ScriptEngine::_instance = NULL;
	
ScriptEngine::ScriptEngine() :
  _initialised(false),
  _openState(NULL)
{
  _instance = this;
}

ScriptEngine::~ScriptEngine() {
  if (_initialised) shutdown();
  _instance = NULL;
}

void ScriptEngine::init() {
  if (_initialised) shutdown();
  // Initialise lua
  _openState = lua_open(0);
  if (!_openState) {
    std::cerr << "Error initialising lua" << std::endl;
    throw std::exception();
  }
  // Init lua base functions
  lua_baselibopen(_openState);
  lua_iolibopen(_openState);
  lua_strlibopen(_openState);
  lua_mathlibopen(_openState);

  script_registerFunctions(_openState);
  _initialised = true;
}

void ScriptEngine::shutdown() {
  // Shutdown lua
  lua_close(_openState);
  _openState = NULL;
  _initialised = false;
}

void ScriptEngine::runCommand(const std::string &command) {
  if (command.empty()) return;
  if (debug) std::cout << "Running: " << command << std::endl;	
  // Ignores any returned values
  int oldtop = lua_gettop(_openState); // Store stack top
  int error = lua_dostring(_openState, command.c_str());
  if (error != 0) printError(error); // Print error messages
  lua_settop(_openState, oldtop); // Restore stack top
}

void ScriptEngine::runFile(const std::string &filename) {
  if (filename.empty()) return;
  if (debug) std::cout << "Running: " << filename << std::endl;	
  // Ignores any returned values
  int oldtop = lua_gettop(_openState); // Store stack top
  int error = lua_dofile(_openState, filename.c_str());
  if (error != 0) printError(error);
  lua_settop(_openState, oldtop); // Restore stack top
}

void ScriptEngine::printError(int error) {
  std::string message;
  switch (error) {
    case (LUA_ERRRUN): message = "Error while running chunk"; break;
    case (LUA_ERRSYNTAX): message = "Syntax error occured during pre-compilation"; break;
    case (LUA_ERRMEM): message = "Memory allocation error"; break;
    case (LUA_ERRFILE): message = "Error opening file"; break;
    default: message = "No errors"; break;
  }
  std::cerr << message << std::endl;
}

void ScriptEngine::event(const std::string &event) {
  int n = lua_gettop(_openState);
  lua_getglobal(_openState, event.c_str());
  // Check that this is a defined function before attempting to exec
  if (lua_isfunction(_openState, n+1) == 1) {
    lua_call(_openState, 0, 0);
  } else {
    lua_remove(_openState, 1);
  }
  lua_settop(_openState, n);
}

void ScriptEngine::event_v(const std::string &event, void *arg1) {
  int n = lua_gettop(_openState);
  lua_getglobal(_openState, event.c_str());
  // Check that this is a defined function before attempting to exec
  if (lua_isfunction(_openState, n+1) == 1) {
    lua_pushusertag(_openState, arg1, LUA_ANYTAG);
    lua_call(_openState, 1, 0);
  } else {
    std::cerr << "Function not found - " << 
	    lua_typename(_openState, 1) << 
	    std::endl;
    lua_remove(_openState, 1);
  }
  lua_settop(_openState, n);
}


void ScriptEngine::event_vs(const std::string &event, void *arg1, const std::string &arg2) {
  int n = lua_gettop(_openState);
  lua_getglobal(_openState, event.c_str());
  // Check that this is a defined function before attempting to exec
  if (lua_isfunction(_openState, n+1) == 1) {
    lua_pushusertag(_openState, arg1, LUA_ANYTAG);
    lua_pushstring(_openState, arg2.c_str());
    lua_call(_openState, 2, 0);
  } else {
    lua_remove(_openState, 1);
  }
  lua_settop(_openState, n);
}

void ScriptEngine::entityEvent(const std::string &id, const std::string &func, const std::string &arglist) {
  std::string cmd = "entityEvent(\"" + id + "\",\"" + func + "\",\"" + arglist + "\");";
  runCommand(cmd);
}
	
} /* namespace Sear */
