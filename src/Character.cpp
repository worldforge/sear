// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: Character.cpp,v 1.31 2004-04-07 00:54:18 alriddoch Exp $

#include <math.h>
#include <string>
#include <SDL/SDL.h>

#include <varconf/Config.h>
#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>
#include <Eris/Avatar.h>

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

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {
  // Console commands	
  static const std::string CHARACTER = "character";
  static const std::string MOVE_FORWARD = "+character_move_forward";
  static const std::string MOVE_BACKWARD = "+character_move_backward";
  static const std::string MOVE_STOP_FORWARD = "-character_move_forward";
  static const std::string MOVE_STOP_BACKWARD = "-character_move_backward";
 
  static const std::string ROTATE_LEFT = "+character_rotate_left";
  static const std::string ROTATE_RIGHT = "+character_rotate_right";
  static const std::string ROTATE_STOP_LEFT = "-character_rotate_left";
  static const std::string ROTATE_STOP_RIGHT = "-character_rotate_right";
 
  static const std::string STRAFE_LEFT = "+character_strafe_left";
  static const std::string STRAFE_RIGHT = "+character_strafe_right";
  static const std::string STRAFE_STOP_LEFT = "-character_strafe_left";
  static const std::string STRAFE_STOP_RIGHT = "-character_strafe_right";

  static const std::string RUN = "+run";
  static const std::string STOP_RUN = "-run";
  static const std::string TOGGLE_RUN = "toggle_run";
  
  static const std::string SAY = "say";
  static const std::string PICKUP = "pickup";
  static const std::string TOUCH = "touch";
  static const std::string DROP = "drop";
  static const std::string GIVE = "give";
  static const std::string DISPLAY_INVENTORY = "inventory"; 

  static const std::string SET_MATERIAL ="set_material";
  static const std::string SET_MESH ="set_mesh";

  static const int server_update_interval = 500;

  // Config section  names
  static const std::string SECTION_character = "character";
  // Config key names
  static const std::string KEY_character_walk_speed = "character_walk_speed";
  static const std::string KEY_character_run_speed = "character_run_speed";
  static const std::string KEY_character_rotate_speed = "character_rotate_speed";

  // Config default values
  static const float DEFAULT_character_walk_speed = 2.0f;
  static const float DEFAULT_character_run_speed = 3.0f;
  static const float DEFAULT_character_rotate_speed = 20.0f;

const float Character::CMD_modifier = 9999.9f;

//actions
static const std::string STOPPED = "stopped_";
static const std::string WALKING = "walking_";
static const std::string RUNNING = "running_";

Character::Character(Eris::Avatar *avatar) :
  _avatar(avatar),
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
  //assert ((self != NULL) && "Character self is NULL");
  _self = (WorldEntity*)_avatar->getEntity();
  _self->Recontainered.connect(SigC::slot(*this, &Character::Recontainered));
}

Character::~Character() {
  if (_initialised) shutdown();
}

bool Character::init() {
  if (_initialised) shutdown();
  float divisor, zaxis;
//  if (!_self) {
//    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
//    return false;
//  }
  readConfig();
  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Character::varconf_callback));
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
  assert ((_initialised == true) && "Character not initialised");	
//  if (!_self) {
//    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
//    return;
//  }
  _speed += speed;
  updateLocals(true);
}

void Character::strafe(float speed) {
  assert ((_initialised == true) && "Character not initialised");	
//  if (!_self) {
//    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
//    return;
//  }
  _strafe_speed += speed;
  updateLocals(true);
}

void Character::rotate(float rate) {
  assert ((_initialised == true) && "Character not initialised");	
//  if (!_self) {
//    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
//    return;
//  }
  std::cout << "Character::rotate" << std::endl << std::flush;
  if (rate != CMD_modifier) _rate += rate * _rotate_speed;
  updateLocals(true);
  // FIXME 0.0f is not safe to test for.
  if (_rate != 0.0f) System::instance()->getEventHandler()->addEvent(Event(EF_UPDATE_CHAR_ROTATE, NULL, EC_TIME, server_update_interval + System::instance()->getTime()));
}

void Character::setMovementSpeed(float speed) {
  assert ((_initialised == true) && "Character not initialised");	

  _speed = speed;
  updateLocals(true);
}

void Character::setStrafeSpeed(float speed) {
  assert ((_initialised == true) && "Character not initialised");	

  _strafe_speed = speed;
  updateLocals(true);
}

void Character::setRotationRate(float rate) {
  assert ((_initialised == true) && "Character not initialised");	

  _rate = rate * _rotate_speed;
  // FIXME 0.0f is not safe to test for.
  if (_rate != 0.0f) System::instance()->getEventHandler()->addEvent(Event(EF_UPDATE_CHAR_ROTATE, NULL, EC_TIME, server_update_interval + System::instance()->getTime()));
}

void Character::updateLocals(bool send_to_server) {
  assert ((_initialised == true) && "Character not initialised");	
  float x, y, z;
  float mod_speed;
  float angle;
  unsigned int ticks;
//  if (!_self) {
//    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
//    return;
//  }
  static ActionHandler *ac = System::instance()->getActionHandler();
  static float old_speed = _speed;
  static bool old_run = _run_modifier;
  static std::string type = _self->type();
  if (old_speed != _speed || old_run != _run_modifier) {
    if (_speed ==  0.0f) {
      ac->handleAction(std::string(STOPPED) + type, NULL);
    } else if (_run_modifier) {
      ac->handleAction(std::string(RUNNING) + type, NULL);
    } else {
      ac->handleAction(std::string(WALKING) + type, NULL);
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
  static const float PI_BY_2 = WFMath::Pi / 2.0f;
  y += mod_speed * -sin(_angle + PI_BY_2);
  x += mod_speed * cos(_angle + PI_BY_2);
  _time = ticks;
  _orient *= WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), -angle);
  if (send_to_server) updateMove(x, y, z, _orient);
}

void Character::updateMove(float x, float y, float z, WFMath::Quaternion orient) {
  assert ((_initialised == true) && "Character not initialised");
  _avatar->moveInDirection(WFMath::Vector<3>(x, y, z), orient);
}

void Character::getEntity(const std::string &id) {
  assert ((_initialised == true) && "Character not initialised");
  
  Eris::EntityPtr e = Eris::World::Instance()->lookup(id);
  if (!e) return;
  _avatar->place(e, _avatar->getEntity());
}

void Character::dropEntity(const std::string &name, int quantity) {
  if (quantity == 0) {
    Log::writeLog( "Quantity is 0! Dropping nothing.", Log::LOG_DEFAULT);
    return;
  }
  Log::writeLog(std::string("Dropping ") + string_fmt(quantity) + std::string(" items of ") + name, Log::LOG_DEFAULT);
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; (quantity) && (i < _self->getNumMembers()); ++i) {
    WorldEntity *we = (WorldEntity*)_avatar->getEntity()->getMember(i);
    if (we->getName() == name) {
      _avatar->drop(we);
      quantity--;
    }
  }
}

void Character::touchEntity(const std::string &id) {
  assert ((_initialised == true) && "Character not initialised");
  Eris::EntityPtr e = Eris::World::Instance()->lookup(id);
  if (!e) return;
  _avatar->touch(e);
}

void Character::displayInventory() {
  assert ((_initialised == true) && "Character not initialised");	
//  if (!_self) return;
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; i < _self->getNumMembers(); ++i)
    inventory[_self->getMember(i)->getName()]++;
  for (std::map<std::string, int>::const_iterator I = inventory.begin(); I != inventory.end(); ++I) {
//    std::cout << I->second << " - " << I->first << std::endl;
  std::string quantity = string_fmt(I->second);
  std::string name = I->first;
    System::instance()->pushMessage(std::string(name + std::string(" - ") + std::string(quantity)), 3);
  }
 
}

void Character::say(const std::string &msg) {
  assert ((_initialised == true) && "Character not initialised");
  _avatar->say(msg);
}

void Character::toggleRunModifier() {
  assert ((_initialised == true) && "Character not initialised");	
//  if (!_self) {
//    Log::writeLog("Character: Error - Character object not created", Log::LOG_ERROR);
//    return;
//  }
  _run_modifier = !_run_modifier;
  updateLocals(true);
}

void Character::readConfig() {
  varconf::Variable temp;
  varconf::Config &general = System::instance()->getGeneral();
  temp = general.getItem(SECTION_character, KEY_character_walk_speed);
  _walk_speed = (!temp.is_double()) ? (DEFAULT_character_walk_speed) : ((double)(temp));
  temp = general.getItem(SECTION_character, KEY_character_run_speed);
  _run_speed = (!temp.is_double()) ? (DEFAULT_character_run_speed) : ((double)(temp));
  temp = general.getItem(SECTION_character, KEY_character_rotate_speed);
  _rotate_speed = (!temp.is_double()) ? (DEFAULT_character_rotate_speed) : ((double)(temp));
}

void Character::writeConfig() {
  assert ((_initialised == true) && "Character not initialised");	
  varconf::Config &general = System::instance()->getGeneral();
  general.setItem(SECTION_character, KEY_character_walk_speed, _walk_speed);
  general.setItem(SECTION_character, KEY_character_run_speed, _run_speed);
  general.setItem(SECTION_character, KEY_character_rotate_speed, _rotate_speed);
}

void Character::giveEntity(const std::string &name, int quantity, const std::string &target) {
  assert ((_initialised == true) && "Character not initialised");	
  if (quantity == 0) {
    Log::writeLog( "Quantity is 0! Dropping nothing.", Log::LOG_DEFAULT);
    return;
  }
  
  Eris::EntityPtr te = Eris::World::Instance()->lookup(target);
  if(!te) {
    Log::writeLog("No target " + target + " to give " + string_fmt(quantity) + " items of " + name + " to", Log::LOG_DEFAULT);
    return;
  }

  Log::writeLog(std::string("Giving ") + string_fmt(quantity) + std::string(" items of ") + name + std::string(" to ") + target, Log::LOG_DEFAULT);
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; (quantity) && (i < _self->getNumMembers()); ++i) {
    WorldEntity *we = (WorldEntity*)_self->getMember(i);
    if (we->getName() == name) {
      _avatar->place(we, te, _self->GetPos());
      quantity--;
    }
  }
}

void Character::registerCommands(Console *console) {
  assert ((_initialised == true) && "Character not initialised");	
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
  assert ((_initialised == true) && "Character not initialised");	
  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(args);
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
     std::string quantity_str = tokeniser.nextToken();
     std::string item = tokeniser.remainingTokens();
     int quantity = 0;
     cast_stream(quantity_str, quantity); 
     giveEntity(item, quantity, System::instance()->getGraphics()->getRender()->getActiveID());
   }
   else if (command == DROP) {
     std::string quantity_str = tokeniser.nextToken();
     std::string item = tokeniser.remainingTokens();
     int quantity = 0;
     cast_stream(quantity_str, quantity); 
     dropEntity(item, quantity);
   }
   else if (command == PICKUP) System::instance()->setAction(ACTION_PICKUP);
   else if (command == TOUCH) System::instance()->setAction(ACTION_TOUCH);
   else if (command == DISPLAY_INVENTORY) displayInventory();
//   else if (command == SET_MESH) {
 //   ObjectHandler *object_handler = _system->getObjectHandler();
  //  ObjectRecord *object_record = object_handler->getObjectRecord(we->getID());
   // object_record->action(command + " " + args);
 // }
}
void Character::Recontainered(Eris::EntityPtr entity1, Eris::EntityPtr entity2) {
  assert ((_initialised == true) && "Character not initialised");	
  if (debug) {
    std::cout << "Recontainered" << std::endl;
    if (entity1) std::cout << "Entity1: " << entity1->getType()->getName() << std::endl;
    if (entity2) std::cout << "Entity2: " << entity2->getType()->getName() << std::endl;
  }
  if (entity2) {
    System::instance()->getActionHandler()->handleAction(std::string("entering_") + entity2->getType()->getName(), NULL);
  }
}

void Character::varconf_callback(const std::string &key, const std::string &section, varconf::Config &config) {
  assert ((_initialised == true) && "Character not initialised");	
  varconf::Variable temp;
  if (section == CHARACTER) {
    if (key == KEY_character_walk_speed) {
      temp = config.getItem(CHARACTER, KEY_character_walk_speed);
      _walk_speed = (!temp.is_double()) ? (DEFAULT_character_walk_speed) : ((double)(temp));
    }
    else if (key == KEY_character_run_speed) {
      temp = config.getItem(CHARACTER, KEY_character_run_speed);
      _run_speed = (!temp.is_double()) ? (DEFAULT_character_run_speed) : ((double)(temp));
    }
    else if (key == KEY_character_rotate_speed) {
      temp = config.getItem(CHARACTER, KEY_character_rotate_speed);
      _rotate_speed = (!temp.is_double()) ? (DEFAULT_character_rotate_speed) : ((double)(temp));
    }
  }
}

} /* namespace Sear */
