// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: System.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_ENGINE_SYSTEM_H
#define SEAR_ENGINE_SYSTEM_H 1

extern "C" {
  #include <lua.h>
}

#include "SDL.h"


namespace Sear {

// Forward Declarations
class EventHandler;
class Server;
class ScriptEngine;
	
class System {
public:
  System();
  ~System();

  void init();
  void shutdown();
  
  void initSDL();
  void shutdownSDL();
  
  void mainLoop();

  void createWindow(unsigned int width, unsigned int height);
  void destroyWindow() { SDL_FreeSurface(_screen); _screen = NULL; }

  /**
   * Register SE commands with engine
   *
   */  
  void registerCommands(lua_State *);
  
  EventHandler *getEventHandler() const { return _event_handler; } 
  Server *getServer() const { return _server; }
  ScriptEngine *getScriptEngine() const { return _script_engine; }
  
  void stopSystem() { _system_running = false; }
  
  unsigned int getTime() { return SDL_GetTicks(); }
  
  static System *instance() { return _instance; }

  
private:
  bool _initialised;
  bool _system_running;
  
  static System *_instance;

  // System Components
  EventHandler *_event_handler;
  Server *_server;
  ScriptEngine *_script_engine;

  SDL_Surface *_screen;

  /**
   * SE command shuts down the system
   */ 
  static int l_quit(lua_State *);
};

} /* namespace Sear */

#endif /* SEAR_ENGINE_SYSTEM_H */
