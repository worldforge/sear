// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Character.cpp,v 1.15 2002-09-08 13:08:21 simon Exp $

#include <math.h>
#include <string>
#include <SDL/SDL.h>

#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Talk.h>

#include <varconf/Config.h>
#include <wfmath/atlasconv.h>
#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "ActionHandler.h"
#include "WorldEntity.h"
#include "EventHandler.h"
#include "Event.h"
#include "System.h"
#include "Character.h"
#include "Console.h"
#include "Render.h"
#include "Graphics.h"

namespace Sear {

Character::Character(WorldEntity *self, System *system) :
  _self(self),
  _system(system),
  _walk_speed(0.0f),
  _run_speed(0.0f),
  _rotate_speed(0.0f),
  _angle(0.0f),
  _rate(0.0f),
  _speed(0.0f),
  _strafe_speed(0.0f),
  _orient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f)),
  _time(0),
  _run_modifier(false),
  _initialised(false)
{
  _self->Recontainered.connect(SigC::slot(*this, &Character::Recontainered));
}

Character::~Character() {
  if (_initialised) shutdown();
}

bool Character::init() {
  if (_initialised) shutdown();
  float divisor, zaxis;
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return false;
  }
  readConfig();
  _orient = _self->getOrientation();
  divisor = sqrt(square(_orient.vector().x()) + square(_orient.vector().y()) + square(_orient.vector().z()));
  zaxis = (divisor != 0.0f) ? (_orient.vector().z() / divisor) : 0.0f; // Check for possible divide by zero error;
  _angle = -2.0f * acos(_self->getOrientation().scalar()) * zaxis;
  _initialised = true;
  return true;
}

void Character::shutdown() {
  writeConfig();
  _initialised = false;
}

void Character::moveForward(float speed) {
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  _speed += speed;
  updateLocals(true);
}

void Character::strafe(float speed) {
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  _strafe_speed += speed;
  updateLocals(true);
}

void Character::rotate(float rate) {
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  if (rate != CMD_modifier) _rate += rate * _rotate_speed;
  updateLocals(true);
  if (_rate != 0.0f) _system->getEventHandler()->addEvent(Event(EF_UPDATE_CHAR_ROTATE, NULL, EC_TIME, server_update_interval + System::instance()->getTime()));
}

void Character::updateLocals(bool send_to_server) {
  float x, y, z;
  float mod_speed;
  float angle;
  unsigned int ticks;
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  static ActionHandler *ac = System::instance()->getActionHandler();
  static float old_speed = _speed;
  static bool old_run = _run_modifier;
  static std::string type = _self->type();
  if (old_speed != _speed || old_run != _run_modifier) {
    if (_speed ==  0.0f) {
      ac->handleAction(std::string("stopped_") + type, NULL);
    } else if (_run_modifier) {
      ac->handleAction(std::string("running_") + type, NULL);
    } else {
      ac->handleAction(std::string("walking_") + type, NULL);
    }
    old_speed = _speed;
    old_run = _run_modifier;
  }
  ticks = SDL_GetTicks();
  angle = deg_to_rad(_rate * ((ticks - _time) / 1000.0f));
  _angle += angle;
//  if (_angle == WFMath::Pi) _angle += 0.01; // Stops entity points due west which causes cyphesis to flip it upside down;
  mod_speed = (_run_modifier) ? (_speed * _run_speed) : (_speed * _walk_speed);
  z = 0.0f;
  y = mod_speed * -sin(_angle);
  x = mod_speed * cos(_angle);  
  mod_speed = (_run_modifier) ? (_strafe_speed * _run_speed) : (_strafe_speed * _walk_speed);
  z += 0.0f;
  y += mod_speed * -sin(_angle + (WFMath::Pi / 2.0f));
  x += mod_speed * cos(_angle + (WFMath::Pi / 2.0f));
  _time = ticks;
  _orient *= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), -angle);
  if (send_to_server) updateMove(x, y, z, _orient);
}

void Character::updateMove(float x, float y, float z, WFMath::Quaternion orient) {
  Atlas::Objects::Operation::Move move;
  Atlas::Message::Object::MapType args;
  Atlas::Message::Object::ListType vel;
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  move = Atlas::Objects::Operation::Move::Instantiate();
  vel.push_back(x);
  vel.push_back(y);
  vel.push_back(z);
  args["velocity"] = vel;
  args["orientation"] = orient.toAtlas();
  args["loc"] = _self->getContainer()->getID();
  args["id"] = _self->getID();
  move.SetFrom(_self->getID());
  move.SetArgs(Atlas::Message::Object::ListType(1, args));
  Eris::Connection::Instance()->send(move);
}

void Character::getEntity(const std::string &id) {
  Atlas::Objects::Operation::Move move;
  Atlas::Message::Object::MapType args;
  Atlas::Message::Object::ListType pos;
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  move = Atlas::Objects::Operation::Move::Instantiate();
  pos.push_back(0.0);
  pos.push_back(0.0);
  pos.push_back(0.0);
  args["pos"] = pos;
  args["loc"] = _self->getID();
  args["id"] = id;
  move.SetFrom(_self->getID());
  move.SetArgs(Atlas::Message::Object::ListType(1, args));
  Eris::Connection::Instance()->send(move);
}

void Character::dropEntity(const std::string &name, int quantity) {
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  if (quantity == 0) {
    Log::writeLog( "Quantity is 0! Dropping nothing.", Log::LOG_DEFAULT);
    return;
  }
  Log::writeLog(std::string("Dropping ") + string_fmt(quantity) + std::string(" items of ") + name, Log::LOG_DEFAULT);
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; (quantity) && (i < _self->getNumMembers()); ++i) {
    WorldEntity *we = (WorldEntity*)_self->getMember(i);
    if (we->getName() == name) {
      Atlas::Objects::Operation::Move move;
      Atlas::Message::Object::MapType args;
      Atlas::Message::Object::ListType pos;
      move = Atlas::Objects::Operation::Move::Instantiate();
      pos.push_back(_self->GetPos().x());
      pos.push_back(_self->GetPos().y());
      pos.push_back(_self->GetPos().z());
      args["pos"] = pos;
      args["loc"] = _self->getContainer()->getID();
      args["id"] = we->getID();
      move.SetFrom(_self->getID());
      move.SetArgs(Atlas::Message::Object::ListType(1, args));
      Eris::Connection::Instance()->send(move);
      quantity--;
    }
  }
}

void Character::touchEntity(const std::string &id) {
  Atlas::Objects::Operation::Touch touch;
  Atlas::Message::Object::MapType args;
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  touch = Atlas::Objects::Operation::Touch::Instantiate();
  args["id"] = id;
  touch.SetFrom(_self->getID());
  touch.SetTo(id);
  touch.SetArgs(Atlas::Message::Object::ListType(1, args));
  Eris::Connection::Instance()->send(touch);
}

void Character::displayInventory() {
  if (!_self) return;
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; i < _self->getNumMembers(); ++i)
    inventory[_self->getMember(i)->getName()]++;
  for (std::map<std::string, int>::const_iterator I = inventory.begin(); I != inventory.end(); ++I) {
//    std::cout << I->second << " - " << I->first << std::endl;
  std::string quantity = string_fmt(I->second);
  std::string name = I->first;
    _system->pushMessage(std::string(name + std::string(" - ") + std::string(quantity)), 3);
  }
 
}

void Character::say(const std::string &msg) {
  Atlas::Objects::Operation::Talk talk;
  Atlas::Message::Object::MapType args;
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  talk =  Atlas::Objects::Operation::Talk::Instantiate();
  args["say"] = msg;
  talk.SetArgs(Atlas::Message::Object::ListType(1, args));
  talk.SetFrom(_self->getID());
  Eris::Connection::Instance()->send(talk);
}

void Character::toggleRunModifier() {
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  _run_modifier = !_run_modifier;
  updateLocals(true);
}

void Character::readConfig() {
  varconf::Variable temp;
  varconf::Config *general = _system->getGeneral();
  if (!general) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  temp = general->getItem("character", KEY_character_walk_speed);
  _walk_speed = (!temp.is_double()) ? (DEFAULT_character_walk_speed) : ((double)(temp));
  temp = general->getItem("character", KEY_character_run_speed);
  _run_speed = (!temp.is_double()) ? (DEFAULT_character_run_speed) : ((double)(temp));
  temp = general->getItem("character", KEY_character_rotate_speed);
  _rotate_speed = (!temp.is_double()) ? (DEFAULT_character_rotate_speed) : ((double)(temp));
}

void Character::writeConfig() {
  varconf::Config *general = _system->getGeneral();
  if (!general) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  general->setItem("character", KEY_character_walk_speed, _walk_speed);
  general->setItem("character", KEY_character_run_speed, _run_speed);
  general->setItem("character", KEY_character_rotate_speed, _rotate_speed);
}

void Character::giveEntity(const std::string &name, int quantity, const std::string &target) {
  if (!_self) {
    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
    return;
  }
  if (quantity == 0) {
    Log::writeLog( "Quantity is 0! Dropping nothing.", Log::LOG_DEFAULT);
    return;
  }
  Log::writeLog(std::string("Giving ") + string_fmt(quantity) + std::string(" items of ") + name + std::string(" to ") + target, Log::LOG_DEFAULT);
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; (quantity) && (i < _self->getNumMembers()); ++i) {
    WorldEntity *we = (WorldEntity*)_self->getMember(i);
    if (we->getName() == name) {
      Atlas::Objects::Operation::Move move;
      Atlas::Message::Object::MapType args;
      Atlas::Message::Object::ListType pos;
      move = Atlas::Objects::Operation::Move::Instantiate();
      pos.push_back(_self->GetPos().x());
      pos.push_back(_self->GetPos().y());
      pos.push_back(_self->GetPos().z());
      args["pos"] = pos;
      args["loc"] = target;
      args["id"] = we->getID();
      move.SetFrom(_self->getID());
      move.SetArgs(Atlas::Message::Object::ListType(1, args));
      Eris::Connection::Instance()->send(move);
      quantity--;
    }
  }
}

void Character::registerCommands(Console *console) {
  console->registerCommand(MOVE_FORWARD, this);
  console->registerCommand(MOVE_BACKWARD, this);
  console->registerCommand(MOVE_STOP_FORWARD, this);
  console->registerCommand(MOVE_STOP_BACKWARD, this);

  console->registerCommand(ROTATE_LEFT, this);
  console->registerCommand(ROTATE_RIGHT, this);
  console->registerCommand(ROTATE_STOP_LEFT, this);
  console->registerCommand(ROTATE_STOP_RIGHT, this);

  console->registerCommand(STRAFE_LEFT, this);
  console->registerCommand(STRAFE_RIGHT, this);
  console->registerCommand(STRAFE_STOP_LEFT, this);
  console->registerCommand(STRAFE_STOP_RIGHT, this);

  console->registerCommand(RUN, this);
  console->registerCommand(STOP_RUN, this);
  console->registerCommand(TOGGLE_RUN, this);

  console->registerCommand(PICKUP, this);
  console->registerCommand(DROP, this);
  console->registerCommand(GIVE, this);
  console->registerCommand(DISPLAY_INVENTORY, this);
  console->registerCommand(TOUCH, this);
  console->registerCommand(SAY, this);
}

void Character::runCommand(const std::string &command, const std::string &args) {
   std::deque<std::string> tokens;
   if (command == MOVE_FORWARD) moveForward(1);
   else if (command == MOVE_BACKWARD) moveForward(-1);
   else if (command == MOVE_STOP_FORWARD) moveForward(-1);
   else if (command == MOVE_STOP_BACKWARD) moveForward( 1);
   
   else if (command == ROTATE_LEFT) rotate(-1);
   else if (command == ROTATE_RIGHT) rotate( 1);
   else if (command == ROTATE_STOP_LEFT) rotate( 1);
   else if (command == ROTATE_STOP_RIGHT) rotate(-1);

   else if (command == STRAFE_LEFT) strafe(-1);
   else if (command == STRAFE_RIGHT) strafe( 1);
   else if (command == STRAFE_STOP_LEFT) strafe( 1);
   else if (command == STRAFE_STOP_RIGHT) strafe(-1); 

   else if (command == RUN || command == STOP_RUN || command == TOGGLE_RUN) toggleRunModifier();

   else if (command == SAY) say(args);
   else if (command == GIVE) {
     int quantity = 0;
     cast_stream(tokens.front(), quantity);
     tokens.pop_front();
     std::string item = tokens.front(); tokens.pop_front();
     while (!tokens.empty()) {
       item += " ";
       item += tokens.front(); tokens.pop_front();
     }
     giveEntity(item, quantity, _system->getGraphics()->getRender()->getActiveID());
   }
   else if (command == DROP) {
     int quantity = 0;
     cast_stream(tokens.front(), quantity); 
     tokens.pop_front();
     std::string item = tokens.front(); tokens.pop_front();
     while (!tokens.empty()) {
       item += " ";
       item += tokens.front(); tokens.pop_front();
     }
     dropEntity(item, quantity);
   }
   else if (command == PICKUP) _system->setAction(ACTION_PICKUP);
   else if (command == TOUCH) _system->setAction(ACTION_TOUCH);
   else if (command == DISPLAY_INVENTORY) displayInventory();
}

void Character::Recontainered(Eris::Entity *entity1, Eris::Entity *entity2) {
//  cout << "Recontainered" << endl;
//  if (entity1) cout << "Entity1: " << entity1->getType()->getName() << endl;
//  if (entity2) cout << "Entity2: " << entity2->getType()->getName() << endl;
  if (entity2) {
    System::instance()->getActionHandler()->handleAction(std::string("entering_") + entity2->getType()->getName(), NULL);
  }
}

} /* namespace Sear */
