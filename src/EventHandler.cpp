// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: EventHandler.cpp,v 1.6 2002-10-21 20:09:59 simon Exp $

#include "EventHandler.h"
#include "Event.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

void EventHandler::addEvent(Event event) {
  _events.push_back(event);
}

void EventHandler::init() {
  if (_initialised) shutdown();
  _initialised = true;
}

void EventHandler::shutdown() {
  while (!_events.empty()) {
    _events.erase(_events.begin());
  }
  _initialised = false;
}

void EventHandler::poll() {
  std::list<Event>::iterator I;
  Event event;
  for (I = _events.begin(); I != _events.end(); ++I) {
    event = *I;
    if (event.checkFireCondition()) {
      event.doEvent();
      _events.erase(I--);
      if (_events.empty()) break;
    }
  }
}

} /* namespace Sear */
