// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: EventHandler.cpp,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#include "EventHandler.h"

#include <iostream>

#include "Bindings.h"
#include "System.h"

#include "script/ScriptEngine.h"

namespace Sear {

static const bool debug = false;
	
EventHandler::EventHandler() :
  _initialised(false)
{}

EventHandler::~EventHandler() {
  if (_initialised) shutdown();
}

void EventHandler::init() {
  if (_initialised) shutdown();
  if (debug) std::cout << "Initialising Event Handler" << std::endl;
  SDL_SetEventFilter(&EventHandler::eventFilter);
  _initialised = true;
}

void EventHandler::shutdown() {
  if (debug) std::cout << "Shutting down Event Handler" << std::endl;
  _initialised = false;
}

void EventHandler::handleEvent(const SDL_Event &event) {
  if (debug) std::cout << "Handling Event" << std::endl;
  switch (event.type) {
    case (SDL_ACTIVEEVENT): handleActiveEvent(event.active); break;
    case (SDL_KEYDOWN): handleKeyDown(event.key); break;
    case (SDL_KEYUP): handleKeyUp(event.key); break;
    case (SDL_MOUSEMOTION): handleMouseMove(event.motion); break;
    case (SDL_MOUSEBUTTONDOWN): handleMouseDown(event.button); break;
    case (SDL_MOUSEBUTTONUP): handleMouseUp(event.button); break;
    case (SDL_JOYAXISMOTION): handleJoyAxisMotion(event.jaxis); break;
    case (SDL_JOYBALLMOTION): handleJoyBallMotion(event.jball); break;
    case (SDL_JOYHATMOTION): handleJoyHatMotion(event.jhat); break;
    case (SDL_JOYBUTTONDOWN): handleJoyButtonDown(event.jbutton); break;
    case (SDL_JOYBUTTONUP): handleJoyButtonUp(event.jbutton); break;
    case (SDL_QUIT): handleQuit(event.quit); break;
    case (SDL_SYSWMEVENT): handleSysWMEvent(event.syswm); break;
    case (SDL_VIDEORESIZE): handleVideoResize(event.resize); break;
    case (SDL_VIDEOEXPOSE): handleVideoExpose(event.expose); break;
    case (SDL_USEREVENT): handleUserEvent(event.user); break;			    default: std::cerr << "Uknown Event" << std::endl; break;
  }
}

void EventHandler::handleActiveEvent(const SDL_ActiveEvent &event) {
  if (debug) {
    std::cout << "Handling ActiveEvent" << std::endl;
    std::cout << "Gain: " << (int)event.gain << std::endl;
    std::cout << "State: " << (int)event.state << std::endl;
  }
}

void EventHandler::handleKeyDown(const SDL_KeyboardEvent &event) {
  if (debug) {
    std::cout << "Handling KeyDown event" << std::endl;
    std::cout << "State: " << (int)event.state << std::endl;
    std::cout << "Sym: " << (char)event.keysym.sym << std::endl;
  }
  ScriptEngine::instance()->runCommand(Bindings::getBinding(Bindings::idToString((int)event.keysym.sym)));
	    
}

void EventHandler::handleKeyUp(const SDL_KeyboardEvent &event) {
  if (debug) {
    std::cout << "Handling KeyUp event" << std::endl;
    std::cout << "State: " << (int)event.state << std::endl;
    std::cout << "Sym: " << (char)event.keysym.sym << std::endl;
  }
}

void EventHandler::handleMouseMove(const SDL_MouseMotionEvent &event) {
  if (debug) std::cout << "Handling MouseMove event" << std::endl;

}

void EventHandler::handleMouseDown(const SDL_MouseButtonEvent &event) {
  if (debug) std::cout << "Handling MouseDown event" << std::endl;
}

void EventHandler::handleMouseUp(const SDL_MouseButtonEvent &event) {
  if (debug) std::cout << "Handling MouseUp event" << std::endl;
}

void EventHandler::handleJoyAxisMotion(const SDL_JoyAxisEvent &event) {
  if (debug) std::cout << "Handling JoyAxisMotion event" << std::endl;
}

void EventHandler::handleJoyBallMotion(const SDL_JoyBallEvent &event) {
  if (debug) std::cout << "Handling JoyBallMotion event" << std::endl;
}

void EventHandler::handleJoyHatMotion(const SDL_JoyHatEvent &event) {
  if (debug) std::cout << "Handling JoyHatMotion event" << std::endl;
}

void EventHandler::handleJoyButtonDown(const SDL_JoyButtonEvent &event) {
  if (debug) std::cout << "Handling JoyButtonDown event" << std::endl;
}

void EventHandler::handleJoyButtonUp(const SDL_JoyButtonEvent &event) {
  if (debug) std::cout << "Handling JoyButtonUp event" << std::endl;
}

void EventHandler::handleQuit(const SDL_QuitEvent &event) {
  if (debug) std::cout << "Handling Quit event" << std::endl;
  System::instance()->stopSystem();
}

void EventHandler::handleSysWMEvent(const SDL_SysWMEvent &event) {
  if (debug) std::cout << "Handling SysWMEvent event" << std::endl;
}

void EventHandler::handleVideoResize(const SDL_ResizeEvent &event) {
  if (debug) std::cout << "Handling VideoResize event" << std::endl;
  std::cout << "Stack Top: " << lua_gettop(ScriptEngine::instance()->getState()) << std::endl;
}

void EventHandler::handleVideoExpose(const SDL_ExposeEvent &event) {
  if (debug) std::cout << "Handling VideoExpose event" << std::endl;
}

void EventHandler::handleUserEvent(const SDL_UserEvent &event) {
  if (debug) std::cout << "Handling UserEvent event" << std::endl;
}

int EventHandler::eventFilter(const SDL_Event *event) {
  switch (event->type) {
    case (SDL_KEYDOWN): return 1;
    case (SDL_KEYUP): return 1;
    case (SDL_QUIT): return 1;
    case (SDL_VIDEORESIZE): return 1;
    default: return 0;
  }
}

} /* namespace Sear */
