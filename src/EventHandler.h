// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: EventHandler.h,v 1.5 2002-10-20 13:22:26 simon Exp $

#ifndef SEAR_EVENTHANDLER_H
#define SEAR_EVENTHANDLER_H 1

#include <list>

#include "Event.h"

namespace Sear {

class System;

class EventHandler {
public:	
  EventHandler() :
    _initialised(false),
    _events(std::list<Event>())
  {}

  void init();
  void shutdown();
    
  void addEvent(Event event);
  void poll();
  
protected:
  bool _initialised;
  std::list<Event> _events;

};

} /* namespace Sear */
#endif /* SEAR_EVENTHANDLER_H */
