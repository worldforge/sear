// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: Character.cpp,v 1.95 2007-05-02 20:47:55 simon Exp $

#include <math.h>
#include <string>
#include <SDL/SDL.h>

#include <sigc++/object_slot.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <varconf/Config.h>

#include <wfmath/atlasconv.h>

#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>
#include <Eris/Avatar.h>
#include <Eris/Account.h>
#include <Eris/View.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "ActionHandler.h"
#include "WorldEntity.h"
#include "client.h"
#include "System.h"
#include "Character.h"
#include "Console.h"
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"
#include "loaders/ObjectHandler.h"
#include "loaders/ObjectRecord.h"

#include "loaders/ModelSystem.h"

using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

static std::string getNameOrType(Sear::WorldEntity *we) {
  std::string str = we->getName();
  if (str.empty()) {
    str = we->type();
  }
  return str;
}

// Console commands
static const std::string CMD_MOVE_FORWARD = "+character_move_forward";
static const std::string CMD_MOVE_BACKWARD = "+character_move_backward";
static const std::string CMD_MOVE_STOP_FORWARD = "-character_move_forward";
static const std::string CMD_MOVE_STOP_BACKWARD = "-character_move_backward";

static const std::string CMD_MOVE_UPWARD = "+character_move_upwards";
static const std::string CMD_MOVE_DOWNWARD = "+character_move_downwards";
static const std::string CMD_MOVE_STOP_UPWARD = "-character_move_upwards";
static const std::string CMD_MOVE_STOP_DOWNWARD = "-character_move_downwards";

static const std::string CMD_ROTATE_LEFT = "+character_rotate_left";
static const std::string CMD_ROTATE_RIGHT = "+character_rotate_right";
static const std::string CMD_ROTATE_STOP_LEFT = "-character_rotate_left";
static const std::string CMD_ROTATE_STOP_RIGHT = "-character_rotate_right";

static const std::string CMD_STRAFE_LEFT = "+character_strafe_left";
static const std::string CMD_STRAFE_RIGHT = "+character_strafe_right";
static const std::string CMD_STRAFE_STOP_LEFT = "-character_strafe_left";
static const std::string CMD_STRAFE_STOP_RIGHT = "-character_strafe_right";

static const std::string CMD_RUN = "+run";
static const std::string CMD_STOP_RUN = "-run";
static const std::string CMD_TOGGLE_RUN = "toggle_run";

static const std::string CMD_SAY = "say";
static const std::string CMD_ME = "me";
static const std::string CMD_PICKUP = "pickup";
static const std::string CMD_TOUCH = "touch";
static const std::string CMD_DROP = "drop";
static const std::string CMD_EAT = "eat";
static const std::string CMD_GIVE = "give";
static const std::string CMD_DISPLAY_INVENTORY = "inventory";
static const std::string CMD_MAKE = "make";
static const std::string CMD_USE = "use";
static const std::string CMD_IDLE = "idle";
static const std::string CMD_WIELD = "wield";
static const std::string CMD_ATTACK = "attack";
static const std::string CMD_DISPLAY_USE_OPS = "use_ops";
  
static const std::string CMD_SET_HEIGHT = "set_height";
static const std::string CMD_RENAME_ENTITY = "rename_entity";

static const std::string CMD_MOVE_TO = "move_to";
static const std::string CMD_MOVE_TO_ORIGIN = "return_to_origin";

static const unsigned int server_update_interval = 500;

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

//actions
static const std::string STOPPED = "ch_stopped_";
static const std::string WALKING = "ch_walking_";
static const std::string RUNNING = "ch_running_";

static const std::string HEIGHT = "height";

namespace Sear {

Character::Character() :
  m_avatar(NULL),
  m_walk_speed(0.0f),
  m_run_speed(0.0f),
  m_rotate_speed(0.0f),
  m_rate(0.0f),
  m_speed(0.0f),
  m_up_speed(0.0f),
  m_strafe_speed(0.0f),
  m_lastUpdate(SDL_GetTicks()),
  m_time(0),
  m_run_modifier(false),
  m_initialised(false),
  m_refresh_orient(true)
{
}

Character::~Character() {
  if (m_initialised) shutdown();
}

bool Character::init() {
  assert (m_initialised == false);

  System::instance()->getGeneral().sigsv.connect(sigc::mem_fun(this, &Character::varconf_callback));

  m_pred_orient.identity();


  m_initialised = true;
  return true;
}

void Character::shutdown() {
  assert (m_initialised == true);

  notify_callbacks();

  m_initialised = false;
}

void Character::moveUpward(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_up_speed += speed;

  // Clamp speed
  if (m_up_speed >  1.0f) m_up_speed =  1.0f;
  if (m_up_speed < -1.0f) m_up_speed = -1.0f;
}

void Character::moveForward(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_speed += speed;

  // Clamp speed
  if (m_speed > 1.0f) m_speed = 1.0f;
  if (m_speed < -1.0f) m_speed = -1.0f;
}

void Character::strafe(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_strafe_speed += speed;

  // Clamp speed
  if (m_strafe_speed >  1.0f) m_strafe_speed =  1.0f;
  if (m_strafe_speed < -1.0f) m_strafe_speed = -1.0f;
}

void Character::rotate(float rate) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_rate += rate;

  if (m_rate >  1.0f) m_rate =  1.0f;
  if (m_rate < -1.0f) m_rate = -1.0f;
}

void Character::rotateImmediate(float rot) {
  assert (m_initialised == true);
  if (!m_avatar) return;

  float angle = deg_to_rad(rot);
  
  m_pred_orient *= WFMath::Quaternion(2,angle);
  dynamic_cast<WorldEntity*>(m_self.get())->setLocalOrient(m_pred_orient);
}

void Character::sendUpdate() {
  if (!m_avatar) return;
  // Send update of our rotation etc to server.
   updateLocals(false);
}

void Character::setMovementSpeed(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_speed = speed;
}

void Character::setUpwardSpeed(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_up_speed = speed;
}

void Character::setStrafeSpeed(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_strafe_speed = speed;
}

void Character::setRotationRate(float rate) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_rate = rate;
}

void Character::updateLocals(bool send_to_server) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  assert(m_self.get() != NULL);

  bool changed = false;
  // TODO: Make into member variables
  static float old_speed = m_speed;
  static float old_strafe_speed = m_strafe_speed;
  static bool old_run = m_run_modifier;
  static WFMath::Quaternion oldOrient = m_pred_orient;

  if (old_speed != m_speed || old_run != m_run_modifier || old_strafe_speed != m_strafe_speed) {
    changed = true;
  }
  unsigned int ticks = System::instance()->getTime();
  float a = deg_to_rad(m_rate * m_rotate_speed * ((ticks - m_time) / 1000.0f));

  // Forward/Backward Speed
  float x_mod_speed = (m_run_modifier) ? (m_speed * m_run_speed) : (m_speed * m_walk_speed);
  // Stafe Speed
  float y_mod_speed = (m_run_modifier) ? (m_strafe_speed * m_run_speed) : (m_strafe_speed * m_walk_speed);
  // New velocity vector
  WFMath::Vector<3> vel(x_mod_speed, y_mod_speed, m_up_speed);

  // Rotate velocity to current heading
  vel = vel.rotate(m_pred_orient);

  dynamic_cast<WorldEntity*>(m_self.get())->setLocalOrient(m_pred_orient);

  // If there is anything to rotate, do so
  if (fabs(a) > 0.000001f) {
    const WFMath::Quaternion angle(2, a);

    m_pred_orient *= angle;
    changed = true;
  }
  if (m_pred_orient != oldOrient) {
    changed = true;
  }

  m_time = ticks;

   // Only send update on a change
  if (changed) {
    // See how long its been since the last server update
    bool send = ((ticks - m_lastUpdate) > server_update_interval);
    // Update if interval has passed, or the force flag is set.
    if (send || send_to_server) {
//      printf("MoveOp: %f, %f, %f\n", vel.x(), vel.y(), vel.z());
      updateMove(vel, m_pred_orient);
      m_lastUpdate = ticks;

      oldOrient = m_pred_orient;
      old_speed = m_speed;
      old_strafe_speed = m_strafe_speed;
      old_run = m_run_modifier;
      
    }
  }
}

void Character::updateMove(const WFMath::Vector<3> & direction,
                           const WFMath::Quaternion & orient) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  m_avatar->moveInDirection(direction, orient);
}

void Character::getEntity(const std::string &id) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  Eris::EntityPtr e = m_avatar->getView()->getEntity(id);
  if (!e) return;
  m_avatar->take(e);
}

void Character::dropEntity(const std::string &name, int quantity) {
  if (!m_avatar) return;

  if (quantity == 0) {
    Log::writeLog( "Quantity is 0! Dropping nothing.", Log::LOG_DEFAULT);
    return;
  }
  if (debug) printf("Dropping %d items of %s\n", quantity, name.c_str());
  // Randomize drop position
  WFMath::Vector<3> pos(
    (float)rand() / (float)RAND_MAX * 2.0f - 1.0f,
    (float)rand() / (float)RAND_MAX * 2.0f - 1.0f,
    0.0f);

  WorldEntity* we = findInInventory(name);
  while (we != 0 && quantity > 0) {
    m_avatar->drop(we, pos);
    --quantity;
    we = findInInventory(name);
  }
}

void Character::touchEntity(const std::string &id) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  Eris::EntityPtr e = m_avatar->getView()->getEntity(id);
  if (!e) return;
  m_avatar->touch(e);
}

void Character::wieldEntity(const std::string &name) {
  if (!m_avatar) return;
  WorldEntity* toWield = findInInventory(name);
  if (!toWield) {
    Log::writeLog("no " + name + " in inventory to wield", Log::LOG_DEFAULT);
    return;
  }
  
  m_avatar->wield(toWield);
}

void Character::eatEntity(const std::string & name) {
  if (!m_avatar) return;
  WorldEntity* toEat = findInInventory(name);
  if (!toEat) {
    Log::writeLog("no " + name + " in inventory to eat", Log::LOG_DEFAULT);
    return;
  }

  Atlas::Objects::Entity::Anonymous food;
  food->setId(toEat->getId());
  Atlas::Objects::Operation::Generic eat;
  eat->setType("eat", -1);
  eat->setFrom(m_self->getId());
  eat->setArgs1(food);
  m_avatar->getConnection()->send(eat);
}

WorldEntity* Character::findInInventory(const std::string& name) {
    assert(m_avatar);
    
    for (unsigned int i = 0; i < m_self->numContained(); ++i) {
        Eris::Entity* e = m_self->getContained(i);
        if ((e->getId() == name)
         || (e->getName() == name) 
         || (e->getType()->getName() == name)) {
            return static_cast<WorldEntity*>(e);
        }
    } // of inventory iteration
    
    return NULL;
}

void Character::useToolOnEntity(const std::string & id,
                                const WFMath::Point<3> & pos) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  if (id.empty()) return;
  Eris::EntityPtr e = m_avatar->getView()->getEntity(id);
  if (!e) return;

  m_avatar->useOn(e, pos, std::string());
}

void Character::attackEntity(const std::string& id) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  if (id.empty()) return;
  Eris::EntityPtr e = m_avatar->getView()->getEntity(id);
  if (!e) return;

  m_avatar->attack(e);
}

void Character::becomeIdle() {
  m_avatar->useStop();
}

void Character::displayUseOperations() {
  if (!m_avatar) return;
  
  //const Eris::TypeInfoArray& ops = m_avatar->getUseOperationsForWielded();
  Eris::EntityRef we = dynamic_cast<WorldEntity*>(m_self.get())->getAttachments().begin()->second;
  if (!we) return;
  const Eris::TypeInfoArray& ops = we->getUseOperations();
  for (unsigned int i=0; i<ops.size(); ++i) {
    System::instance()->pushMessage(ops[i]->getName(), 1);
  }
}

void Character::displayInventory() {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; i < m_self->numContained(); ++i) {
    WorldEntity *we = dynamic_cast<WorldEntity*>(m_self->getContained(i));
    inventory[getNameOrType(we)]++;
  }
  for (std::map<std::string, int>::const_iterator I = inventory.begin(); 
                                                  I != inventory.end(); ++I) {
    std::string quantity = string_fmt(I->second);
    std::string name = I->first;
    System::instance()->pushMessage(std::string(name + std::string(" - ") + std::string(quantity)), 3);
  }
}

void Character::say(const std::string &msg) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  m_avatar->say(msg);
}

void Character::emote(const std::string &msg) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  m_avatar->emote(msg);
}

void Character::make(const std::string &type, const std::string &name) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  Atlas::Objects::Operation::Create c;
  c->setFrom(m_self->getId());
  Anonymous msg;

  WFMath::Vector<3> displacement(
    (float)rand() / (float)RAND_MAX * 4.0f - 2.0f,
    (float)rand() / (float)RAND_MAX * 4.0f - 2.0f,
    0.0f);

  Eris::Entity *e = m_self->getLocation();
  assert(e);
  msg->setLoc(e->getId());
  WFMath::Point<3> pos = m_self->getPosition() + displacement;
  msg->setAttr("pos", pos.toAtlas());
  msg->setName(name);
  msg->setParents(std::list<std::string>(1, type));
  c->setArgs1(msg);
  m_avatar->getConnection()->send(c);
}

void Character::toggleRunModifier() {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  m_run_modifier = !m_run_modifier;
}

void Character::readConfig(varconf::Config &config) {
  assert ((m_initialised == true) && "Character not initialised");
  varconf::Variable temp;

  temp = config.getItem(SECTION_character, KEY_character_walk_speed);
  m_walk_speed = (!temp.is_double()) ? (DEFAULT_character_walk_speed) : ((double)(temp));
  temp = config.getItem(SECTION_character, KEY_character_run_speed);
  m_run_speed = (!temp.is_double()) ? (DEFAULT_character_run_speed) : ((double)(temp));
  temp = config.getItem(SECTION_character, KEY_character_rotate_speed);
  m_rotate_speed = (!temp.is_double()) ? (DEFAULT_character_rotate_speed) : ((double)(temp));
}

void Character::writeConfig(varconf::Config &config) {
  assert ((m_initialised == true) && "Character not initialised");

  config.setItem(SECTION_character, KEY_character_walk_speed, m_walk_speed);
  config.setItem(SECTION_character, KEY_character_run_speed, m_run_speed);
  config.setItem(SECTION_character, KEY_character_rotate_speed, m_rotate_speed);
}

void Character::giveEntity(const std::string &name, int quantity, const std::string &target) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  if (quantity == 0) {
    Log::writeLog( "Quantity is 0! Giving nothing.", Log::LOG_DEFAULT);
    return;
  }

  Eris::EntityPtr te = System::instance()->getClient()->getAvatar()->getView()->getEntity(target);
  if(!te) {
    Log::writeLog("No target " + target + " to give " + string_fmt(quantity) + " items of " + name + " to", Log::LOG_DEFAULT);
    return;
  }

  if (debug) printf("Giving %d items of %s to %s\n", quantity, name.c_str(), target.c_str());

  WFMath::Point<3> pos(0,0,0);
  WorldEntity* we = findInInventory(name);
  while (we != 0 && quantity > 0) {
    m_avatar->place(we, te, pos);
    --quantity;
    we = findInInventory(name);
  }
}

void Character::registerCommands(Console *console) {
  console->registerCommand(CMD_MOVE_FORWARD, this);
  console->registerCommand(CMD_MOVE_BACKWARD, this);
  console->registerCommand(CMD_MOVE_STOP_FORWARD, this);
  console->registerCommand(CMD_MOVE_STOP_BACKWARD, this);

  console->registerCommand(CMD_MOVE_UPWARD, this);
  console->registerCommand(CMD_MOVE_DOWNWARD, this);
  console->registerCommand(CMD_MOVE_STOP_UPWARD, this);
  console->registerCommand(CMD_MOVE_STOP_DOWNWARD, this);

  console->registerCommand(CMD_ROTATE_LEFT, this);
  console->registerCommand(CMD_ROTATE_RIGHT, this);
  console->registerCommand(CMD_ROTATE_STOP_LEFT, this);
  console->registerCommand(CMD_ROTATE_STOP_RIGHT, this);

  console->registerCommand(CMD_STRAFE_LEFT, this);
  console->registerCommand(CMD_STRAFE_RIGHT, this);
  console->registerCommand(CMD_STRAFE_STOP_LEFT, this);
  console->registerCommand(CMD_STRAFE_STOP_RIGHT, this);

  console->registerCommand(CMD_RUN, this);
  console->registerCommand(CMD_STOP_RUN, this);
  console->registerCommand(CMD_TOGGLE_RUN, this);

  console->registerCommand(CMD_PICKUP, this);
  console->registerCommand(CMD_DROP, this);
  console->registerCommand(CMD_EAT, this);
  console->registerCommand(CMD_GIVE, this);
  console->registerCommand(CMD_DISPLAY_INVENTORY, this);
  console->registerCommand(CMD_MAKE, this);
  console->registerCommand(CMD_ME, this);
  console->registerCommand(CMD_TOUCH, this);
  console->registerCommand(CMD_SAY, this);
  console->registerCommand(CMD_USE, this);
  console->registerCommand(CMD_IDLE, this);
  console->registerCommand(CMD_WIELD, this);
  console->registerCommand(CMD_ATTACK, this);
  console->registerCommand(CMD_SET_HEIGHT, this);
  console->registerCommand(CMD_DISPLAY_USE_OPS, this);
  console->registerCommand(CMD_RENAME_ENTITY, this);
  console->registerCommand(CMD_MOVE_TO, this);
  console->registerCommand(CMD_MOVE_TO_ORIGIN, this);
}

void Character::runCommand(const std::string &command, const std::string &args) {
  assert ((m_initialised == true) && "Character not initialised");

  if (!m_avatar) return;

  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(args);
  if (command == CMD_MOVE_FORWARD) moveForward(1);
  else if (command == CMD_MOVE_BACKWARD) moveForward(-1);
  else if (command == CMD_MOVE_STOP_FORWARD) moveForward(-1);
  else if (command == CMD_MOVE_STOP_BACKWARD) moveForward( 1);

  else if (command == CMD_MOVE_UPWARD) moveUpward(1);
  else if (command == CMD_MOVE_DOWNWARD) moveUpward(-1);
  else if (command == CMD_MOVE_STOP_UPWARD) moveUpward(-1);
  else if (command == CMD_MOVE_STOP_DOWNWARD) moveUpward( 1);

  else if (command == CMD_ROTATE_LEFT) rotate( 1);
  else if (command == CMD_ROTATE_RIGHT) rotate(-1);
  else if (command == CMD_ROTATE_STOP_LEFT) rotate(-1);
  else if (command == CMD_ROTATE_STOP_RIGHT) rotate( 1);

  else if (command == CMD_STRAFE_LEFT) strafe( 1);
  else if (command == CMD_STRAFE_RIGHT) strafe(-1);
  else if (command == CMD_STRAFE_STOP_LEFT) strafe(-1);
  else if (command == CMD_STRAFE_STOP_RIGHT) strafe( 1);

  else if (command == CMD_RUN || command == CMD_STOP_RUN || command == CMD_TOGGLE_RUN) toggleRunModifier();

  else if (command == CMD_SAY) say(args);
  else if (command == CMD_ME) emote(args);
  else if (command == CMD_GIVE) {
    const std::string &quantity_str = tokeniser.nextToken();
    const std::string &item = tokeniser.remainingTokens();
    int quantity = 0;
    cast_stream(quantity_str, quantity);
    giveEntity(item, quantity, RenderSystem::getInstance().getRenderer()->getActiveID());
  }
  else if (command == CMD_DROP) {
    const std::string &quantity_str = tokeniser.nextToken();
    const std::string &item = tokeniser.remainingTokens();
    int quantity = 0;
    cast_stream(quantity_str, quantity);
    dropEntity(item, quantity);
  }
  else if (command == CMD_PICKUP) System::instance()->setAction(ACTION_PICKUP);
  else if (command == CMD_TOUCH) System::instance()->setAction(ACTION_TOUCH);
  else if (command == CMD_DISPLAY_INVENTORY) {
//    displayInventory();
    System::instance()->getActionHandler()->handleAction("inventory_open", NULL);
  }
  else if (command == CMD_MAKE) {
    const std::string &type = tokeniser.nextToken();
    std::string name = tokeniser.remainingTokens();
    if (name.empty()) {
        name = type;
    }
    make(type, name);
  }
  else if (command == CMD_USE) {
    System::instance()->setAction(ACTION_USE);
//    useToolOnEntity(RenderSystem::getInstance().getRenderer()->getActiveID());
  }
  else if (command == CMD_IDLE) {
    becomeIdle();
  }
  else if (command == CMD_DISPLAY_USE_OPS) {
    displayUseOperations();
  }
  else if (command == CMD_WIELD) {
    const std::string &name = tokeniser.nextToken();
    wieldEntity(name);
  }
  else if (command == CMD_EAT) {
    const std::string &name = tokeniser.nextToken();
    eatEntity(name);
  }
  else if (command == CMD_ATTACK) {
    System::instance()->setAction(ACTION_ATTACK);
  }
  else if (command == CMD_SET_HEIGHT) {
    const std::string &hStr = tokeniser.nextToken();
    float h;
    cast_stream(hStr, h);
    setHeight(h);
  }
  else if (command ==  CMD_RENAME_ENTITY) {
    const std::string &id = tokeniser.nextToken();
    const std::string &name = tokeniser.remainingTokens();
    WorldEntity *we = findInInventory(id);
    if (we) renameEntity(we, name);
  }
  else if (command == CMD_MOVE_TO_ORIGIN) {
    m_avatar->moveToPoint(WFMath::Point<3>(0,0,0));
  }
  else if (command == CMD_MOVE_TO) {

    const std::string &str_x = tokeniser.nextToken();
    const std::string &str_y = tokeniser.nextToken();
    const std::string &str_z = tokeniser.nextToken();
    float x,y,z;
    cast_stream(str_x, x);
    cast_stream(str_y, y);
    cast_stream(str_z, z);
    m_avatar->moveToPoint(WFMath::Point<3>(x,y,z));
  }
}

void Character::varconf_callback(const std::string &key, const std::string &section, varconf::Config &config) {
  assert ((m_initialised == true) && "Character not initialised");
  varconf::Variable temp;
  if (section == SECTION_character) {
    if (key == KEY_character_walk_speed) {
      temp = config.getItem(SECTION_character, KEY_character_walk_speed);
      m_walk_speed = (!temp.is_double()) ? (DEFAULT_character_walk_speed) : ((double)(temp));
    }
    else if (key == KEY_character_run_speed) {
      temp = config.getItem(SECTION_character, KEY_character_run_speed);
      m_run_speed = (!temp.is_double()) ? (DEFAULT_character_run_speed) : ((double)(temp));
    }
    else if (key == KEY_character_rotate_speed) {
      temp = config.getItem(SECTION_character, KEY_character_rotate_speed);
      m_rotate_speed = (!temp.is_double()) ? (DEFAULT_character_rotate_speed) : ((double)(temp));
    }
  }
}

void Character::setHeight(float height) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  Atlas::Objects::Operation::Set set;
  set->setFrom(System::instance()->getClient()->getAccount()->getId());

  Anonymous msg;
  msg->setId(m_self->getId());
  msg->setObjtype("obj");
  msg->setAttr(HEIGHT, height);

  set->setArgs1(msg);
  m_avatar->getConnection()->send(set);
}

void Character::setAvatar(Eris::Avatar *avatar) {
  m_avatar = avatar;
  if (avatar == NULL) {
    m_self = Eris::EntityRef();
    // TODO we also need to disconnect any signals at this point!
    // Otherwise we can clear the list during a disconnect, and then the
    // ChildRemoved signal can get fired!
    // We can't use notify_callbacks as other signals are used in this class.
    m_imap.clear();
  } else {
    m_self = Eris::EntityRef(m_avatar->getEntity());
    WorldEntity *we = dynamic_cast<WorldEntity*>(m_self.get());

    we->ChildAdded.connect(sigc::mem_fun(this, &Character::onChildAdded)); 
    we->ChildRemoved.connect(sigc::mem_fun(this, &Character::onChildRemoved));
    we->LocationChanged.connect(sigc::mem_fun(this, &Character::onLocationChanged));
    we->Moved.connect(sigc::mem_fun(this, &Character::onMoved));
    m_refresh_orient = true;
    onMoved();

    m_imap.clear();
    for (unsigned int i = 0; i < m_self->numContained(); ++i) {
      onChildAdded(m_self->getContained(i));
    }
  }
}

void Character::getOrientation(WorldEntity *we){
  assert (we != NULL);
  m_pred_orient = we->getEntityOrientation();
}

void Character::onChildAdded(Eris::Entity *child) {
  std::string name = child->getName();
  if (name.empty()) name = child->getType()->getName();

  int count = 0;
  if (m_imap.find(name) != m_imap.end()) {
    count = m_imap[name];
  }
  m_imap[name] = ++count;
}

void Character::onChildRemoved(Eris::Entity *child) {
  // Check for null pointer. On
  if (child == 0) return;
  std::string name = child->getName();
  if (name.empty()) name = child->getType()->getName();

  InventoryMap::iterator I = m_imap.find(name);
  if (I == m_imap.end()) return;

  int count = I->second;
  if (count == 1) {
    m_imap.erase(I);
  } else {
    m_imap[name] = --count;
  }
}

void Character::onLocationChanged(Eris::Entity *loc) {
  // Location changed, get the new orientation when the Moved
  // signal fires.
  m_refresh_orient = true;
}

void Character::onMoved() {
  // Only update the orientation is the location has changed
  if (m_refresh_orient) {
    WorldEntity *we = dynamic_cast<WorldEntity*>(m_self.get());
    m_pred_orient = we->getEntityOrientation();
    
    m_refresh_orient = false;
  }
}

void Character::renameEntity(Eris::Entity *e, const std::string &name) {
  // This function requests a rename op. The inventory is not updated
  // with the new name until the entity is dropped and picked up again
  // as the mapping is only updated on a childadded/removed event.
  // We could perhaps setup a observer here and remove it when it fires,
  // or after a timeout event. (e.g. nothing is returned if the rename fails)

  if (m_avatar == 0) return;
  assert(e != 0);

  Atlas::Objects::Operation::Set set;
  set->setFrom(m_self->getId());

  Anonymous msg;
  msg->setId(e->getId());
  msg->setObjtype("obj");
  msg->setAttr("name", name);

  set->setArgs1(msg);
  m_avatar->getConnection()->send(set);

}

} /* namespace Sear */
