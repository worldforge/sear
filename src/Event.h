// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Event.h,v 1.4 2002-09-26 20:23:03 simon Exp $

#ifndef SEAR_EVENT_H
#define SEAR_EVENT_H 1

#include <string>

namespace Sear {

typedef enum {
  EF_UNKNOWN = 0,
  EF_PRINT,
  EF_RUN_COMMAND,
  EF_HANDLE_MOVE,
  EF_UPDATE_CHAR_ROTATE,
  EF_FREE_MODEL,
  EF_FREE_MODELS
} EventFunction;

typedef enum {
  EC_UNKNOWN = 0,
  EC_TIME,
  EC_CONNECTED,
  EC_LOGGED_IN,
  EC_IN_WORLD
} EventCondition;

class Event {
public:
  Event();
  Event(EventFunction, void*, EventCondition, unsigned int);
  Event(const std::string, const std::string, const std::string, const std::string);
  void doEvent();
  bool checkFireCondition();
  static char *stringToObject(const std::string &msg);
  
protected:
  EventFunction _ef;
  void* _target;
  EventCondition _ec;
  unsigned int _time;
};

} /* namespace Sear */
#endif /* SEAR_EVENT_H */
