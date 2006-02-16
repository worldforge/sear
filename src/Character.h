// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: Character.h,v 1.39 2006-02-16 17:39:44 simon Exp $

#ifndef SEAR_CHARACTER_H
#define SEAR_CHARACTER_H 1


#include <string>

#include <wfmath/quaternion.h>
#include <Eris/Entity.h>
#include <Eris/EntityRef.h>
#include <Eris/Timeout.h>
#include <Eris/Types.h>
#include <Eris/Avatar.h>
#include "interfaces/ConsoleObject.h"
#include <sigc++/trackable.h>

namespace varconf {
class Config;
}

namespace Sear {

// Forward declarations
class Console;
class Client;
class System;
class WorldEntity;

/**
 * This class wraps the functions available to a player character.
 *
 */

class Character : public ConsoleObject, public sigc::trackable {
public:
  /**
   * Constructor.
   */
  Character();

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
  bool isInitialised() const { return m_initialised; }

  void moveForward(float);
  void moveUpward(float);
  void strafe(float);
  void rotate(float);

  /**
   * Rotate the character immediatly by a certain angle.
   * @param Angle in degrees of rotation
   */
  void rotateImmediate(float);
  void sendUpdate();

  void setMovementSpeed(float);
  void setUpwardSpeed(float);
  void setStrafeSpeed(float);
  void setRotationRate(float);

  void updateLocals(bool);
  void updateMove(const WFMath::Vector<3> &, const WFMath::Quaternion &);

  void getEntity(const std::string&);
  void dropEntity(const std::string&, int);
  void giveEntity(const std::string&, int, const std::string&);
  void touchEntity(const std::string&);
  void wieldEntity(const std::string&);
  void useToolOnEntity(const std::string&, const WFMath::Point<3>&);
  void attackEntity(const std::string&);
  void eatEntity(const std::string&);
  void displayInventory();
  void say(const std::string&);
  void emote(const std::string&);
  void make(const std::string&type, const std::string &name);
  void displayUseOperations();

  float getAngle() const { return m_angle; }
//  WFMath::Quaternion getOrientation() { return m_orient; }
  Eris::Avatar *getAvatar() const { return m_avatar; }
//  WorldEntity *getSelf() { return m_self; }
  void toggleRunModifier();

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  void registerCommands(Console*);
  void runCommand(const std::string &, const std::string &);
	
  static const float CMD_modifier;
  void setAppearance(const std::string &, const std::string &, const std::string &);
  void sendGuise(const Atlas::Message::Element& guise);
  void clearApp();
  void setHeight(float);
  void setAction(const std::string &action);

  void setAvatar(Eris::Avatar *avatar);

private:
  /**
  @brief Locate an item in the inventory by instance or type name
  @param name The instance or type name to search for
  Return NULL if no such item can be found, and the first match if multiple
  items are present.
  */
  WorldEntity* findInInventory(const std::string& name);
 
  Eris::Avatar *m_avatar;
  Eris::EntityRef m_self;
  float m_walk_speed;
  float m_run_speed;
  float m_rotate_speed;

  float m_angle;
  float m_pitch;
  float m_rate;
  float m_speed;
  float m_up_speed;
  float m_strafe_speed;

  unsigned int m_lastUpdate;
  bool m_updateScheduled;

//  WFMath::Quaternion m_orient;

  unsigned int m_time; ///< Used to record time since last server update

  bool m_run_modifier; ///< Flag storing run/walk state. True means run

  bool m_initialised; ///< Initialisation state of character object

  void varconf_callback(const std::string &key, const std::string &section, varconf::Config &config);

  void RotateTimeoutExpired();
  void UpdateTimeoutExpired();
  Eris::Timeout *m_timeout_rotate, *m_timeout_update;

};

} /* namespace Sear */

#endif /* SEAR_CHARACTER_H */
