// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Character.h,v 1.9 2002-11-12 23:59:22 simon Exp $

#ifndef SEAR_CHARACTER_H
#define SEAR_CHARACTER_H 1


#include <string>

#include <wfmath/quaternion.h>
#include <Eris/Entity.h>
#include "ConsoleObject.h"
#include <sigc++/object_slot.h>

namespace varconf {
class Config;
}

namespace Sear {

class Client;
class System;
class WorldEntity;

class Character : public ConsoleObject, public SigC::Object {
public:
  Character(WorldEntity*, System*);
  ~Character();

  bool init();
  void shutdown();
  
  void moveForward(float);
  void strafe(float);
  void rotate(float);
  
  void updateLocals(bool);
  void updateMove(float, float, float, WFMath::Quaternion);
  
  void getEntity(const std::string&);
  void dropEntity(const std::string&, int);
  void giveEntity(const std::string&, int, const std::string&);
  void touchEntity(const std::string&);
  void displayInventory();
  void say(const std::string&);
  
  float getAngle() { return _angle; }
  WFMath::Quaternion getOrientation() { return _orient; }
  WorldEntity *getSelf() { return _self; }
  void toggleRunModifier();

  void readConfig();
  void writeConfig();

  void registerCommands(Console*);
  void runCommand(const std::string &, const std::string &);
	
  static const float CMD_modifier = 9999.9f;
  
protected:
  static const int server_update_interval = 500;

  static const char * const KEY_character_walk_speed = "character_walk_speed";
  static const char * const KEY_character_run_speed = "character_run_speed";
  static const char * const KEY_character_rotate_speed = "character_rotate_speed";

  static const float DEFAULT_character_walk_speed = 2.0f;
  static const float DEFAULT_character_run_speed = 3.0f;
  static const float DEFAULT_character_rotate_speed = 20.0f;

  WorldEntity *_self;
  System *_system;
  
  float _walk_speed;
  float _run_speed;
  float _rotate_speed;
  
  float _angle;
  float _rate;
  float _speed;
  float _strafe_speed;

  WFMath::Quaternion _orient;

  unsigned int _time;

  bool _run_modifier;

  void Recontainered(Eris::Entity *, Eris::Entity *);
  bool _initialised;
 
  void varconf_callback(const std::string &key, const std::string &section, varconf::Config &config);
  
private:
  static const char * const MOVE_FORWARD = "+character_move_forward";
  static const char * const MOVE_BACKWARD = "+character_move_backward";
  static const char * const MOVE_STOP_FORWARD = "-character_move_forward";
  static const char * const MOVE_STOP_BACKWARD = "-character_move_backward";
 
  static const char * const ROTATE_LEFT = "+character_rotate_left";
  static const char * const ROTATE_RIGHT = "+character_rotate_right";
  static const char * const ROTATE_STOP_LEFT = "-character_rotate_left";
  static const char * const ROTATE_STOP_RIGHT = "-character_rotate_right";
 
  static const char * const STRAFE_LEFT = "+character_strafe_left";
  static const char * const STRAFE_RIGHT = "+character_strafe_right";
  static const char * const STRAFE_STOP_LEFT = "-character_strafe_left";
  static const char * const STRAFE_STOP_RIGHT = "-character_strafe_right";

  static const char * const RUN = "+run";
  static const char * const STOP_RUN = "-run";
  static const char * const TOGGLE_RUN = "toggle_run";
  
  static const char * const SAY = "say";
  static const char * const PICKUP = "pickup";
  static const char * const TOUCH = "touch";
  static const char * const DROP = "drop";
  static const char * const GIVE = "give";
  static const char * const DISPLAY_INVENTORY = "inventory";
};

} /* namespace Sear */
#endif /* SEAR_CHARACTER_H */
