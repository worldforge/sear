// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ScriptEngine.h,v 1.1.2.2 2003-01-05 18:18:28 simon Exp $

#ifndef SEAR_SCRIPTENGINE_H
#define SEAR_SCRIPTENGINE_H 1

#include <string>

extern "C" {
  #include <lua.h>
}

namespace Sear {

class ScriptEngine {
public:
  ScriptEngine();
  ~ScriptEngine();

  void init();
  void shutdown();

  void runCommand(const std::string &command);
  void runFile(const std::string &filename);

  void printError(int error);

  void event(const std::string &event);
  void event_v(const std::string &event, void *arg1);
  void event_vs(const std::string &event, void *arg1, const std::string &arg2);
 
  void entityEvent(const std::string &id, const std::string &func, const std::string &arglist);
  
  lua_State *getState() const { return _openState; }
  static ScriptEngine *instance() { return _instance; }
private:
  bool _initialised;
  static ScriptEngine *_instance;
  lua_State *_openState;
};
	
} /* namespace Sear */

#endif /* SEAR_SCRIPTENGINE_H */
