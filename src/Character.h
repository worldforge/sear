// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CHARACTER_H_
#define _CHARACTER_H_ 1

#include <string>

//#include <SDL/SDL.h>

#include <wfmath/quaternion.h>

namespace Sear {

class Client;
class WorldEntity;

class Character {
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
  void toggleRunModifier();

  void readConfig();
  void writeConfig();

  static const float CMD_modifier = 9999.9f;
  
protected:
  static const char * const CMD_character_run_args = "0 0 1";
  static const char * const CMD_character_walk_args = "1 0 0";


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
};

} /* namespace Sear */
#endif /* _CHARACTER_H_ */
