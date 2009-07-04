// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall, University of Southampton

#include <math.h>
#include <string>
#include <SDL/SDL.h>

#include <sigc++/object_slot.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Root.h>
#include <varconf/config.h>

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
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"
#include "loaders/ObjectHandler.h"
#include "loaders/ObjectRecord.h"

#include "loaders/ModelSystem.h"

using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

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

  Eris::EntityPtr te = m_avatar->getView()->getEntity(target);
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

bool Character::init(Eris::Avatar *avatar) {

  assert(m_initialised == false);

  System::instance()->getGeneral().sigsv.connect(sigc::mem_fun(this, &Character::varconf_callback));

  m_pred_orient.identity();

  m_avatar = avatar;

  m_self = Eris::EntityRef(m_avatar->getEntity());
  WorldEntity *we = dynamic_cast<WorldEntity*>(m_self.get());

  we->ChildAdded.connect(sigc::mem_fun(this, &Character::onChildAdded)); 
  we->ChildRemoved.connect(sigc::mem_fun(this, &Character::onChildRemoved));
  we->LocationChanged.connect(sigc::mem_fun(this, &Character::onLocationChanged));
  we->Moved.connect(sigc::mem_fun(this, &Character::onMoved));
  m_refresh_orient = true;

  onMoved();

  for (unsigned int i = 0; i < m_self->numContained(); ++i) {
    onChildAdded(m_self->getContained(i));
  }

  m_initialised = true;
  return true;
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


void Character::combineEntity(const std::vector<Eris::Entity *> &e) {

  if (m_avatar == 0) return;

  Atlas::Objects::Operation::Combine op;
  op->setFrom(m_self->getId());

  std::vector<Root> msgs;

  std::vector<Eris::Entity*>::const_iterator I = e.begin();
  std::vector<Eris::Entity*>::const_iterator Iend = e.end();

  while (I != Iend) {
    Root msg;
    Eris::Entity *e = *I;
    msg->setId(e->getId());
    msgs.push_back(msg);
    I++;
  }

  op->setArgs(msgs);
  m_avatar->getConnection()->send(op);
}

void Character::divideEntity(Eris::Entity *e, int num) {

  if (m_avatar == 0) return;

  Atlas::Objects::Operation::Divide op;
  op->setFrom(m_self->getId());

  Root msg;
  msg->setId(e->getId());
  msg->setAttr("num", num);

  op->setArgs1(msg);
  m_avatar->getConnection()->send(op);
}

} /* namespace Sear */
