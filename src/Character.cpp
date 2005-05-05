// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: Character.cpp,v 1.60 2005-05-05 20:26:31 simon Exp $

#include <math.h>
#include <string>
#include <SDL/SDL.h>

#include <Atlas/Objects/Operation.h>
#include <varconf/Config.h>

#include <wfmath/atlasconv.h>

#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>
#include <Eris/Avatar.h>
#include <Eris/Account.h>
#include <Eris/View.h>
#include <Eris/DeleteLater.h>

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

#include "common/operations.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

//  Wrap angle around it required.
float limitAngle(float a) {
  static float p2 = M_PI * 2.0;
  while (a > M_PI) a -= p2;
  while (a < -M_PI) a += p2;
  return a;
}

namespace Sear {
  // Console commands
  static const std::string CMD_MOVE_FORWARD = "+character_move_forward";
  static const std::string CMD_MOVE_BACKWARD = "+character_move_backward";
  static const std::string CMD_MOVE_STOP_FORWARD = "-character_move_forward";
  static const std::string CMD_MOVE_STOP_BACKWARD = "-character_move_backward";

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
  static const std::string CMD_PICKUP = "pickup";
  static const std::string CMD_TOUCH = "touch";
  static const std::string CMD_DROP = "drop";
  static const std::string CMD_GIVE = "give";
  static const std::string CMD_DISPLAY_INVENTORY = "inventory";
  static const std::string CMD_MAKE = "make";
  static const std::string CMD_USE = "use";
  static const std::string CMD_WIELD = "wield";

  static const std::string CMD_SET_APPEARANCE = "set_app";
  static const std::string CMD_RESET_APPEARANCE = "clear_app";
  static const std::string CMD_READ_APPEARANCE = "read_app";
  static const std::string CMD_SET_HEIGHT = "set_height";
  static const std::string CMD_SET_ACTION = "set_action";
  static const std::string CMD_WAVE = "wave";

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
static const std::string STOPPED = "ch_stopped_";
static const std::string WALKING = "ch_walking_";
static const std::string RUNNING = "ch_running_";

static const std::string GUISE = "guise";
static const std::string HEIGHT = "height";

Character::Character() :
  m_avatar(NULL),
  m_self(NULL),
  m_walk_speed(0.0f),
  m_run_speed(0.0f),
  m_rotate_speed(0.0f),
  m_angle(0.0f),
  m_rate(0.0f),
  m_speed(0.0f),
  m_strafe_speed(0.0f),
  m_lastUpdate(SDL_GetTicks()),
  m_updateScheduled(false),
  m_time(0),
  m_run_modifier(false),
  m_initialised(false),
  m_timeout_rotate(NULL),
  m_timeout_update(NULL)
{
}

Character::~Character() {
  assert (m_initialised == false);
}

bool Character::init() {
  assert (m_initialised == false);

  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Character::varconf_callback));

  m_timeout_rotate = new Eris::Timeout("sear_character_rotate", this, 0);
  m_timeout_rotate->Expired.connect(SigC::slot(*this, &Character::RotateTimeoutExpired));

  m_initialised = true;
  return true;
}

void Character::shutdown() {
  assert (m_initialised == true);

  // Cleanup timeouts
  if (m_timeout_update) {
    delete m_timeout_update;
  }
  if (m_timeout_rotate) {
    delete m_timeout_rotate;
  }

  m_initialised = false;
}

void Character::moveForward(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_speed += speed;
  updateLocals(true);
}

void Character::strafe(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_strafe_speed += speed;
  updateLocals(true);
}

void Character::rotate(float rate) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  if (debug) std::cout << "Character::rotate" << std::endl << std::flush;
  if (rate != CMD_modifier) m_rate += rate * m_rotate_speed;
  updateLocals(true);

  if (m_rate > 0.000001f &&  m_timeout_rotate->isExpired()) {
    m_timeout_rotate->reset(server_update_interval);
  }
}

void Character::rotateImmediate(float rot)
{
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  float angle = deg_to_rad(rot);
  m_angle += angle;
  // Only send to server if we haven't recently.
  bool send = ((SDL_GetTicks() - m_lastUpdate) > 1000);
  updateLocals(send);
  // If we don't send, we need to schedule an update.
  if (!send && !m_updateScheduled) {
    m_timeout_update = new Eris::Timeout("sear_character_update", this, server_update_interval);
    m_timeout_update->Expired.connect(SigC::slot(*this, &Character::UpdateTimeoutExpired));

    m_updateScheduled = true;
  }
}

void Character::sendUpdate()
{
  if (!m_avatar) return;
  // Send update of our rotation etc to server.
  updateLocals(true);
  m_updateScheduled = false;
}

void Character::setMovementSpeed(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_speed = speed;
  updateLocals(true);
}

void Character::setStrafeSpeed(float speed) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_strafe_speed = speed;
  updateLocals(true);
}

void Character::setRotationRate(float rate) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;

  m_rate = rate * m_rotate_speed;
  if (m_rate > 0.000001f && m_timeout_rotate->isExpired()) {
    m_timeout_rotate->reset(server_update_interval);
  }
}

void Character::updateLocals(bool send_to_server) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  assert(m_self != NULL);
  float x, y, z;
  float mod_speed;
  float angle;
  unsigned int ticks;

  static ActionHandler *ac = System::instance()->getActionHandler();
  static float old_speed = m_speed;
  static bool old_run = m_run_modifier;
  std::string type = m_self->type();

  //float divisor, zaxis;
//  m_orient = m_self->getOrientation();
//  divisor = sqrt(square(m_orient.vector().x()) + square(m_orient.vector().y()) + square(m_orient.vector().z()));
//  zaxis = (divisor != 0.0f) ? (m_orient.vector().z() / divisor) : 0.0f; // Check for possible divide by zero error;
//  m_angle = -2.0f * acos(m_self->getOrientation().scalar()) * zaxis;

  if (old_speed != m_speed || old_run != m_run_modifier) {
    if (m_speed ==  0.0f) {
      ac->handleAction(std::string(STOPPED) + type, NULL);
    } else if (m_run_modifier) {
      ac->handleAction(std::string(RUNNING) + type, NULL);
    } else {
      ac->handleAction(std::string(WALKING) + type, NULL);
    }
    old_speed = m_speed;
    old_run = m_run_modifier;
  }
  ticks = SDL_GetTicks();
  angle = deg_to_rad(m_rate * ((ticks - m_time) / 1000.0f));
  m_angle += angle;

  // m_angle = limitAngle(m_angle);

//  if (_angle == WFMath::Pi) _angle += 0.01; // Stops entity points due west which causes cyphesis to flip it upside down;
  mod_speed = (m_run_modifier) ? (m_speed * m_run_speed) : (m_speed * m_walk_speed);
  z = 0.0f;
  y = mod_speed * -sin(m_angle);
  x = mod_speed * cos(m_angle);
  mod_speed = (m_run_modifier) ? (m_strafe_speed * m_run_speed) : (m_strafe_speed * m_walk_speed);
  z += 0.0f;
  static const float PI_BY_2 = WFMath::Pi / 2.0f;
  y += mod_speed * -sin(m_angle + PI_BY_2);
  x += mod_speed * cos(m_angle + PI_BY_2);
  m_time = ticks;
  WFMath::Quaternion m_orient = WFMath::Quaternion(WFMath::Vector<3>(0.0f, 0.0f, 1.0f), -m_angle);
  if (send_to_server) {
    updateMove(WFMath::Vector<3>(x, y, z), m_orient);
    m_lastUpdate = ticks;
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

  Eris::EntityPtr e = System::instance()->getClient()->getAvatar()->getView()->getEntity(id);
  if (!e) return;
  m_avatar->place(e, m_avatar->getEntity());
  setAction("pickup");
}

void Character::dropEntity(const std::string &name, int quantity) {
  if (!m_avatar) return;

  if (quantity == 0) {
    Log::writeLog( "Quantity is 0! Dropping nothing.", Log::LOG_DEFAULT);
    return;
  }
  Log::writeLog(std::string("Dropping ") + string_fmt(quantity) + std::string(" items of ") + name, Log::LOG_DEFAULT);
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; (quantity) && (i < m_self->numContained()); ++i) {
    WorldEntity *we = static_cast<WorldEntity*>(m_avatar->getEntity()->getContained(i));
    if (we->getName() == name) {
      m_avatar->drop(we,WFMath::Vector<3>(1.0f, 0.0f, 0.0f));
      quantity--;
    }
  }
  setAction("drop");
}

void Character::touchEntity(const std::string &id) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  Eris::EntityPtr e = m_avatar->getView()->getEntity(id);
  if (!e) return;
  m_avatar->touch(e);
  setAction("touch");
}

void Character::wieldEntity(const std::string &name) {
  if (!m_avatar) return;
  for (unsigned int i = 0; i < m_self->numContained(); ++i) {
    WorldEntity *we = static_cast<WorldEntity*>(m_avatar->getEntity()->getContained(i));
    Eris::TypeInfo *type = we->getType();
    assert(type);
    if ((we->getName() == name) || (type->getName() == name)) {
      Wield w;
      w->setFrom(m_self->getId());
      Atlas::Message::MapType arg;
      arg["id"] = we->getId();
      arg["objtype"] = "obj";
      w->setArgsAsList(Atlas::Message::ListType(1, arg));
      m_avatar->getConnection()->send(w);
      setAction("wield");
      return;
    }
  }
}

void Character::useToolOnEntity(const std::string & id,
                                const WFMath::Point<3> & pos) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  if (id.empty()) return;
  Eris::EntityPtr e = m_avatar->getView()->getEntity(id);
  if (!e) return;
  Use u;
  u->setFrom(m_self->getId());
  Atlas::Message::MapType arg;
  arg["id"] = e->getId();
  arg["objtype"] = "obj";
  arg["pos"] = pos.toAtlas();
  u->setArgsAsList(Atlas::Message::ListType(1, arg));
  m_avatar->getConnection()->send(u);
  setAction("use");
}

void Character::displayInventory() {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; i < m_self->numContained(); ++i) {
    inventory[m_self->getContained(i)->getName()]++;
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

void Character::make(const std::string &type, const std::string &name) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  Atlas::Objects::Operation::Create c;
  c->setFrom(m_self->getId());
  Atlas::Message::MapType msg;
  
  Eris::Entity *e = m_self->getLocation();
  assert(e);
  msg["loc"] = e->getId();
  WFMath::Point<3> pos = m_self->getPosition() + WFMath::Vector<3>(2,0,0);
  msg["pos"] = pos.toAtlas();
  msg["name"] = name;
  msg["parents"] = Atlas::Message::ListType(1, type);
  c->setArgsAsList(Atlas::Message::ListType(1, msg));
  m_avatar->getConnection()->send(c);
}

void Character::toggleRunModifier() {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  m_run_modifier = !m_run_modifier;
  updateLocals(true);
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
    Log::writeLog( "Quantity is 0! Dropping nothing.", Log::LOG_DEFAULT);
    return;
  }

  Eris::EntityPtr te = System::instance()->getClient()->getAvatar()->getView()->getEntity(target);
  if(!te) {
    Log::writeLog("No target " + target + " to give " + string_fmt(quantity) + " items of " + name + " to", Log::LOG_DEFAULT);
    return;
  }

  Log::writeLog(std::string("Giving ") + string_fmt(quantity) + std::string(" items of ") + name + std::string(" to ") + target, Log::LOG_DEFAULT);
  std::map<std::string, int> inventory;
  for (unsigned int i = 0; (quantity) && (i < m_self->numContained()); ++i) {
    WorldEntity *we = (WorldEntity*)m_self->getContained(i);
    if (we->getName() == name) {
      m_avatar->place(we, te, WFMath::Point<3>(0,0,0));
      quantity--;
    }
  }
  setAction("give");
}

void Character::registerCommands(Console *console) {
  console->registerCommand(CMD_MOVE_FORWARD, this);
  console->registerCommand(CMD_MOVE_BACKWARD, this);
  console->registerCommand(CMD_MOVE_STOP_FORWARD, this);
  console->registerCommand(CMD_MOVE_STOP_BACKWARD, this);

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
  console->registerCommand(CMD_GIVE, this);
  console->registerCommand(CMD_DISPLAY_INVENTORY, this);
  console->registerCommand(CMD_MAKE, this);
  console->registerCommand(CMD_TOUCH, this);
  console->registerCommand(CMD_SAY, this);
  console->registerCommand(CMD_USE, this);
  console->registerCommand(CMD_WIELD, this);
  console->registerCommand(CMD_SET_APPEARANCE, this);
  console->registerCommand(CMD_RESET_APPEARANCE, this);
  console->registerCommand(CMD_READ_APPEARANCE, this);
  console->registerCommand(CMD_SET_HEIGHT, this);
  console->registerCommand(CMD_SET_ACTION, this);
  console->registerCommand(CMD_WAVE, this);
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

  else if (command == CMD_ROTATE_LEFT) rotate(-1);
  else if (command == CMD_ROTATE_RIGHT) rotate( 1);
  else if (command == CMD_ROTATE_STOP_LEFT) rotate( 1);
  else if (command == CMD_ROTATE_STOP_RIGHT) rotate(-1);

  else if (command == CMD_STRAFE_LEFT) strafe(-1);
  else if (command == CMD_STRAFE_RIGHT) strafe( 1);
  else if (command == CMD_STRAFE_STOP_LEFT) strafe( 1);
  else if (command == CMD_STRAFE_STOP_RIGHT) strafe(-1);

  else if (command == CMD_RUN || command == CMD_STOP_RUN || command == CMD_TOGGLE_RUN) toggleRunModifier();

  else if (command == CMD_SAY) say(args);
  else if (command == CMD_GIVE) {
    std::string quantity_str = tokeniser.nextToken();
    std::string item = tokeniser.remainingTokens();
    int quantity = 0;
    cast_stream(quantity_str, quantity);
    giveEntity(item, quantity, RenderSystem::getInstance().getRenderer()->getActiveID());
  }
  else if (command == CMD_DROP) {
    std::string quantity_str = tokeniser.nextToken();
    std::string item = tokeniser.remainingTokens();
    int quantity = 0;
    cast_stream(quantity_str, quantity);
    dropEntity(item, quantity);
  }
  else if (command == CMD_PICKUP) System::instance()->setAction(ACTION_PICKUP);
  else if (command == CMD_TOUCH) System::instance()->setAction(ACTION_TOUCH);
  else if (command == CMD_DISPLAY_INVENTORY) displayInventory();
  else if (command == CMD_MAKE) {
    std::string type = tokeniser.nextToken();
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
  else if (command == CMD_WIELD) {
    std::string name = tokeniser.nextToken();
    wieldEntity(name);
  }
  else if (command == CMD_RESET_APPEARANCE) clearApp();
  else if (command == CMD_SET_HEIGHT) {
    std::string hStr = tokeniser.nextToken();
    float h;
    cast_stream(hStr, h);
    setHeight(h);
  }
  else if (command == CMD_SET_APPEARANCE) {
    std::string map = tokeniser.nextToken();
    std::string name = tokeniser.nextToken();
    std::string value = tokeniser.remainingTokens();
    setAppearance(map, name, value);
  }
  else if (command == CMD_READ_APPEARANCE) {
    ObjectHandler *object_handler = ModelSystem::getInstance().getObjectHandler();
    Atlas::Message::MapType mt;
    ObjectRecord *record = NULL;
    if (object_handler) record = object_handler->getObjectRecord(m_self->getId());
    if (m_self->hasAttr(GUISE)) { // Read existing values
      mt = m_self->valueOfAttr(GUISE).asMap();
      if (record) record->setAppearance(mt);
    } else { // Set defaults and send to server
      if (record) {
        record->setAppearance(mt);
        m_self->setAttr(GUISE, mt);
        setApp();
      }
    }
  }
  else if (command == CMD_SET_ACTION) {
    setAction(args);
  }
  else if (command == CMD_WAVE) {
    setAction("wave");
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

void Character::setAppearance(const std::string &map, const std::string &name, const std::string &value) {
  if (!m_avatar) return;
  if (!name.empty()) {
    Atlas::Message::MapType mt;
    if (m_self->hasAttr(GUISE)) {
      mt = m_self->valueOfAttr(GUISE).asMap();
    }
    Atlas::Message::MapType::iterator I = mt.find(map);
    if (I != mt.end()) {
      I->second.asMap()[name] = value;
    } else {
      Atlas::Message::MapType m;
      m[name] = value;
      mt[map] = m;
    }
    m_self->setAttr(GUISE, mt);
  }
  setApp();
}

void Character::clearApp() {
  assert ((m_initialised == true) && "Character not initialised");
  Atlas::Objects::Operation::Set set;
  set->setFrom(System::instance()->getClient()->getAccount()->getId());

  Atlas::Message::MapType msg;
  const Atlas::Message::MapType mt;
  msg["id"] = m_self->getId();
  msg["objtype"] = "obj";
  msg[GUISE] = mt;

  set->setArgsAsList(Atlas::Message::ListType(1, msg));
  m_avatar->getConnection()->send(set);
}

void Character::setApp() {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  Atlas::Objects::Operation::Set set;
  set->setFrom(System::instance()->getClient()->getAccount()->getId());

  Atlas::Message::MapType msg;
  const Atlas::Message::MapType mt = m_self->valueOfAttr(GUISE).asMap();
  msg["id"] = m_self->getId();
  msg["objtype"] = "obj";
  msg[GUISE] = mt;

  set->setArgsAsList(Atlas::Message::ListType(1, msg));
  m_avatar->getConnection()->send(set);
}

void Character::setHeight(float height) {
  assert ((m_initialised == true) && "Character not initialised");
  if (!m_avatar) return;
  Atlas::Objects::Operation::Set set;
  set->setFrom(System::instance()->getClient()->getAccount()->getId());

  Atlas::Message::MapType msg;
  msg["id"] = m_self->getId();
  msg["objtype"] = "obj";
  msg[HEIGHT] = height;

  set->setArgsAsList(Atlas::Message::ListType(1, msg));
  m_avatar->getConnection()->send(set);
}

void Character::setAction(const std::string &action) {
  assert ((m_initialised == true) && "Character not initialised");
//  init();
  Atlas::Objects::Operation::Set set;
  set->setFrom(m_self->getId());

  Atlas::Message::MapType msg;
  msg["id"] = m_self->getId();
  msg["objtype"] = "obj";
  msg["action"] = action;

  set->setArgsAsList(Atlas::Message::ListType(1, msg));
  m_avatar->getConnection()->send(set);
}

void Character::RotateTimeoutExpired() {
  rotate(CMD_modifier);
}
void Character::UpdateTimeoutExpired() {
  Eris::deleteLater(m_timeout_update);
  m_timeout_update = NULL;
  sendUpdate();
}

void Character::setAvatar(Eris::Avatar *avatar) {
  m_avatar = avatar;
  if (avatar == NULL) {
    m_self = NULL;
  } else {
    m_self = dynamic_cast<WorldEntity*>(m_avatar->getEntity());
    assert(m_self != NULL);

    WFMath::Vector<3> q = m_self->getAbsOrient().vector();
    WFMath::CoordType w = m_self->getAbsOrient().scalar();

    float v1,v2;
    // Calculate attitude (which way we are facing)
    v1  = 2.0f * q.x() * q.y() + 2.0f * q.z() * w;
    float attitude = asin(v1);
    // Attitude is now in the range -pi/2 -> pi/2

    // Calculate bank so we can adjust attitude into the range
    // -pi -> +pi
    // Heading also exhibits the same behaviour, however should the angle of the
    // z axis change, heading may be preferrable to bank. We shall have to see.
    v1 = 2.0f * q.x() * w - 2.0f * q.y() * q.z();
    v2 = 1.0f - 2.0f * q.x() * q.x()- 2.0f * q.z() * q.z();

    float bank = atan2(v1, v2);

    // Adjust attitude as neccessary
    // bank actually flips to +=M_PI, but it is safer to test this way.
    if  (bank > M_PI_2) attitude  = M_PI - attitude;
    if  (bank < -M_PI_2) attitude  = -M_PI - attitude;

    m_angle = attitude;
  }
}
} /* namespace Sear */
