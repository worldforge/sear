// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: System.cpp,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#include "System.h"

#include "SDL.h"

#include "EventHandler.h"
#include "script/ScriptEngine.h"

#include "engine/Server.h"

#include "Bindings.h"
#include "WorldEntity.h"

#include <unistd.h>

namespace Sear {

static const bool debug = true;
	
System *System::_instance = NULL;
	
System::System() :
  _initialised(false),
  _system_running(false),
  _event_handler(NULL),
  _server(NULL),
  _script_engine(NULL),
  _screen(NULL)
{
  _instance = this;
}
	
System::~System() {
  if (_initialised) shutdown();
  _instance = NULL;
}

void System::init() {
  if (_initialised) shutdown();
  if (debug) std::cout << "Initialising System" << std::endl;
  initSDL();
  // initialise system components
  _event_handler = new EventHandler();
  _event_handler->init();
  _script_engine = new ScriptEngine();
  _script_engine->init();
  _server = new Server(); 
  _server->init();
  // Register scriptengine functions
  registerCommands(_script_engine->getState());
  _server->registerCommands(_script_engine->getState());
  WorldEntity::registerCommands(_script_engine->getState());
  // Setup keyboard bindings
  Bindings::init();
  Bindings::registerCommands(_script_engine->getState());

  _initialised = true;
}

void System::shutdown() {
  if (debug) std::cout << "Shutting down System" << std::endl;
  if (_server) {
    _server->shutdown();
    delete _server;
    _server = NULL;
  }
  if (_script_engine) {
    _script_engine->shutdown();
    delete _script_engine;
    _script_engine = NULL;
  }

  if (_event_handler) {
    _event_handler->shutdown();
    delete _event_handler;
    _event_handler = NULL;
  }
  shutdownSDL();
  _initialised = false;
}
 
void System::initSDL() {
  if (debug) std::cout << "Initialising SDL" << std::endl;
  if ((SDL_Init(SDL_INIT_VIDEO) == -1)) {
    std::cerr << "Error initialising SDL - " << SDL_GetError() <<  std::endl;
    return;
  }
  createWindow(640, 480);
}

void System::shutdownSDL() {
  if (debug) std::cout << "Shutting down SDL" << std::endl;
  SDL_Quit();
}


void System::mainLoop() {
  _system_running = true;
  SDL_Event event;
  // Loop until _system_running is set to false
  while (_system_running) {
    usleep(1000);
    // Check for network updates
    _server->poll();
    // Process any input devices
    while(SDL_PollEvent(&event))
      _event_handler->handleEvent(event);
  }
}
  
void System::createWindow(unsigned int width, unsigned int height) {
  if (_screen) destroyWindow();
  //Request Open GL window attributes
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1 );
  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if (!info) {
    std::cerr << "Error quering video - " << SDL_GetError() << std::endl;
    return;
  }
  //Create Window
  int flags = SDL_OPENGL | SDL_VIDEORESIZE;
  int bpp = info->vfmt->BitsPerPixel;
  if (!(width && height)) {
    std::cerr << "Invalid resolution: "  << width  << " x " << height << std::endl;
    return;
  }
  if (debug) std::cout <<"Setting video to " << width << " x " << height << std::endl;

  _screen = SDL_SetVideoMode(width, height, bpp, flags);
  if (!_screen) {
    std::cerr <<"Unable to set video to " << width << " x " << height << "- " << SDL_GetError() <<  std::endl;
  }
}

void System::registerCommands(lua_State *L) {
  lua_register(L, "quit", &System::l_quit);
}

int System::l_quit(lua_State *L) {
  _instance->stopSystem();
  return 0;
}

} /* namespace Sear */
