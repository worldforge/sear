// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_ 1

#include <list>

#include "Event.h"

namespace Sear {

class System;

class EventHandler {
public:	
  EventHandler (System *system) :
    _events(std::list<Event>()),	  
    _system(system)
  {}

  void addEvent(Event event);
  void poll();
  
protected:
  std::list<Event> _events;
  System *_system;

};

} /* namespace Sear */
#endif /* _EVENTHANDLER_H_ */
