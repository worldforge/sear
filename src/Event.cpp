// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "System.h"
#include "Event.h"
#include "WorldEntity.h"

#include <Eris/World.h>
#include <iostream.h>
#include <string>
#include "Render.h"
#include "Character.h"

namespace Sear {

Event::Event() :
  _ef(EF_UNKNOWN),
  _target(0),
  _ec(EC_UNKNOWN),
  _time(0)
{}

Event::Event(EventFunction ef, void* target, EventCondition ec, unsigned int time) :
  _ef(ef),
  _target(target),
  _ec(ec),
  _time(time)
{}
 
Event::Event(const std::string ef, const std::string target, const std::string ec, const std::string time) {
  _ef = (EventFunction)atoi(ef.c_str());
  if (_ef == EF_PRINT || _ef == EF_RUN_COMMAND) _target = stringToObject(target);
  else if (_ef == EF_HANDLE_MOVE) _target = Eris::World::Instance()->lookup(target);
  _ec = (EventCondition)atoi(ec.c_str());
 _time = atoi(time.c_str());
 if (_ec == EC_TIME) _time += System::instance()->getTime(); 
}
  
void Event::doEvent() {
  switch (_ef) {
    case (EF_UNKNOWN): {
      break;
    }
    case (EF_PRINT): {
      char* msg = (char*)_target;
      std::cout << msg << std::endl;
      free(_target);
      break;
    }	  
    case (EF_RUN_COMMAND): {
      char *cmd = (char*)_target;
      System::instance()->runCommand(cmd);
      break;      
    }
    case (EF_HANDLE_MOVE): {
      ((WorldEntity*)_target)->handleMove();
      break;
    }
    case (EF_UPDATE_CHAR_ROTATE): {
      System::instance()->getCharacter()->rotate(Character::CMD_modifier);
      break;      
    }
    case (EF_FREE_MODEL): {
      System::instance()->getRenderer()->checkModelStatus(std::string((char *)_target));
      free(_target);
      break;
    }
    default: break;
  }
}

bool Event::checkFireCondition() {
  switch (_ec) {
    case (EC_UNKNOWN): break;
    case (EC_TIME): {		 
      if (System::instance()->getTime() > _time) return true;
      break;
    }
    case (EC_CONNECTED): {
      if (System::instance()->checkState(SYS_CONNECTED) == _time) return true;
      break;
    }
    case (EC_LOGGED_IN): {
      if (System::instance()->checkState(SYS_LOGGED_IN) == _time) return true;
      break;
    }
    case (EC_IN_WORLD): {
      if (System::instance()->checkState(SYS_IN_WORLD) == _time) return true;
      break;
    }
    default: break;

  }
  return false;
}

char *Event::stringToObject(const std::string &msg) {
  // Not the best way to do this
  // Ideally will remove this
  unsigned int i;
  char *string = (char*)malloc(sizeof(char) * msg.size() + 1);
  for (i = 0; i < msg.size(); i++) string[i] = msg[i];
  string[msg.size()] = '\0';
  return string;
}

}
