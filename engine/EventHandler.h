// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: EventHandler.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_ENGINE_EVENTHANDLER_H
#define SEAR_ENGINE_EVENTHANDLER_H 1

#include "SDL.h"

namespace Sear {

class EventHandler {
public:
  EventHandler();
  ~EventHandler();

  void init();
  void shutdown();
  
  /**
   * This function passes an incoming event to the correct
   * handler function
   */ 
  void handleEvent(const SDL_Event &event);
  // Specific event handlers
  void handleActiveEvent(const SDL_ActiveEvent &event);
  void handleKeyDown(const SDL_KeyboardEvent &event);
  void handleKeyUp(const SDL_KeyboardEvent &event);
  void handleMouseMove(const SDL_MouseMotionEvent &event);
  void handleMouseDown(const SDL_MouseButtonEvent &event);
  void handleMouseUp(const SDL_MouseButtonEvent &event);
  void handleJoyAxisMotion(const SDL_JoyAxisEvent &event);
  void handleJoyBallMotion(const SDL_JoyBallEvent &event);
  void handleJoyHatMotion(const SDL_JoyHatEvent &event);
  void handleJoyButtonDown(const SDL_JoyButtonEvent &event);
  void handleJoyButtonUp(const SDL_JoyButtonEvent &event);
  void handleQuit(const SDL_QuitEvent &event);
  void handleSysWMEvent(const SDL_SysWMEvent &event);
  void handleVideoResize(const SDL_ResizeEvent &event);
  void handleVideoExpose(const SDL_ExposeEvent &event);
  void handleUserEvent(const SDL_UserEvent &event);

  /**
   * This function is passed to SDL to filter incoming events
   */ 
  static int eventFilter(const SDL_Event *event);
  
private:
  bool _initialised;  
};
	
} /* namespace Sear */

#endif /* SEAR_ENGINE_EVENTHANDLER_H */
