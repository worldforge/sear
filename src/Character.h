// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: Character.h,v 1.15 2004-04-12 15:28:50 alriddoch Exp $

#ifndef SEAR_CHARACTER_H
#define SEAR_CHARACTER_H 1


#include <string>

#include <wfmath/quaternion.h>
#include <Eris/Entity.h>
#include <Eris/Utils.h>
#include <Eris/Types.h>
#include <Eris/Avatar.h>
#include "interfaces/ConsoleObject.h"
#include <sigc++/object_slot.h>

namespace varconf {
class Config;
}

namespace Sear {

// Forward declarations
class Client;
class System;
class WorldEntity;

/**
 * This class wraps the functions available to a player character.
 *
 */

class Character : public ConsoleObject, public SigC::Object {
public:
  /**
   * Constructor.
   * @param we The character entity
   * @param system The system object
   */
  Character(Eris::Avatar *avatar);

  /**
   * Destructor
   */
  ~Character();

  /**
   * Initialise character object
   * @return True on success, false on failure
   */
  bool init();

  /**
   * Shutdown character object
   */
  void shutdown();


  void moveForward(float);
  void strafe(float);
  void rotate(float);

  void setMovementSpeed(float);
  void setStrafeSpeed(float);
  void setRotationRate(float);

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
	
  static const float CMD_modifier;

private:
  WorldEntity *_self;
  Eris::Avatar *_avatar;
  float _walk_speed;
  float _run_speed;
  float _rotate_speed;

  float _angle;
  float _rate;
  float _speed;
  float _strafe_speed;

  WFMath::Quaternion _orient;

  unsigned int _time; ///< Used to record time since last server update

  bool _run_modifier; ///< Flag storing run/walk state. True means run

  void Recontainered(Eris::Entity*, Eris::Entity*);
  bool _initialised; ///< Initialisation state of character object

  void varconf_callback(const std::string &key, const std::string &section, varconf::Config &config);

};

} /* namespace Sear */

#endif /* SEAR_CHARACTER_H */
