// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: EventHandler.h,v 1.4 2002-09-08 00:24:53 simon Exp $

#ifndef SEAR_EVENTHANDLER_H
#define SEAR_EVENTHANDLER_H 1

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
#endif /* SEAR_EVENTHANDLER_H */
