// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _EVENT_H_
#define _EVENT_H_ 1

#include <string>

namespace Sear {

typedef enum {
  EF_UNKNOWN = 0,
  EF_PRINT,
  EF_RUN_COMMAND,
  EF_HANDLE_MOVE,
  EF_UPDATE_CHAR_ROTATE,
  EF_FREE_MODEL
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
#endif /* _EVENT_H_ */
