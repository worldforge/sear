// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: EventHandler.cpp,v 1.8 2004-04-27 15:07:02 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif
#include "EventHandler.h"
#include "Event.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
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
