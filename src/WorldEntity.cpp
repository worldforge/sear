// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: WorldEntity.cpp,v 1.44 2005-01-06 12:46:55 simon Exp $
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <Atlas/Message/Element.h>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include <Eris/Types.h>
#include <Eris/TypeInfo.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "System.h"
#include "Console.h"
#include "Event.h"
#include "EventHandler.h"
#include "renderers/Graphics.h"
#include "loaders/ObjectHandler.h"
#include "loaders/ObjectRecord.h"
#include "loaders/Model.h"
#include "renderers/Render.h"
#include "WorldEntity.h"
#include "ActionHandler.h"

#include "environment/Environment.h"
#include <set>


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

static const std::string ACTION = "action";
static const std::string MODE = "mode";
static const std::string GUISE = "guise";
	
WorldEntity::WorldEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world):
   Eris::Entity(ge, world),
   messages(std::list<message>()),
   m_lastMoveTime(0)
{
  Changed.connect(SigC::slot(*this, &WorldEntity::sigChanged));
}

WorldEntity::~WorldEntity() {
  while (!messages.empty()) {
    messages.erase(messages.begin());
  }
}

void WorldEntity::handleMove()
{
    // record the time this data was updated, so we can interpolate pos
    m_lastMoveTime = System::instance()->getTime();
    
  rotateBBox(getAbsOrient());
}

void WorldEntity::handleTalk(const std::string &msg) {
  Log::writeLog(_id + std::string(": ") + msg, Log::LOG_DEFAULT);	
  System::instance()->pushMessage(_name + ": " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);

  if (messages.size() >= MAX_MESSAGES) messages.erase(messages.begin());
  messages.push_back(message(msg, System::instance()->getTime() + message_life));  
}

bool WorldEntity::hasMessages() {
  return !messages.empty();
}

void WorldEntity::renderMessages() {
  if (messages.empty()) return;
  //Convert messages into char arrays up x characters long
  std::list<std::string> mesgs = std::list<std::string>();
  std::list<screenMessage>::reverse_iterator I;
  for (I = messages.rbegin(); I != messages.rend(); ++I) {
    const std::string str = (const std::string)((*I).first);
    std::list<std::string> message_list = std::list<std::string>();
    unsigned int pos = 0;
    while (pos < str.length()) {
      message_list.push_back( std::string(str, pos, pos + string_size));
      pos+=string_size;
    }

    std::list<std::string>::reverse_iterator K;
    for (K = message_list.rbegin(); K != message_list.rend(); ++K) {
      mesgs.push_back(*K);
    }
     
  }
  // Render text strings
  Render *renderer = System::instance()->getGraphics()->getRender();
  std::list<std::string>::iterator J;
  for (J = mesgs.begin(); J != mesgs.end(); ++J) { 
    std::string str = (*J);
    renderer->newLine();
    renderer->print3D((char*)str.c_str(), 0);
  }
  //Print all character arrays
  bool loop = true;
  unsigned int current_time = System::instance()->getTime();
  while (loop) {
    if (messages.empty()) break;
    loop = false;
    message sm = *messages.begin();
    unsigned int message_time = sm.second;
    if (current_time > message_time) {
      messages.erase(messages.begin());
      loop = true;
    }
  }
}

const WFMath::Quaternion WorldEntity::getAbsOrient() 
{
    WFMath::Quaternion parentOrient(1.0f, 0.0f, 0.0f, 0.0f);
    if (getContainer()) parentOrient = static_cast<WorldEntity*>(getContainer())->getAbsOrient();
    return parentOrient / getOrientation();
}

const WFMath::Point<3> WorldEntity::getAbsPos()
{
    WorldEntity* loc = static_cast<WorldEntity*>(getContainer());
    if (!loc) return getPosition(); // nothing below makes sense for the world.
    
    WFMath::Point<3> absPos = loc->getAbsPos() + 
        getInterpolatedPos().rotate(loc->getAbsOrient().inverse());
    
  // Set Z coord to terrain height if required

  if (hasProperty(MODE)) {
    std::string mode = getProperty(MODE).asString();
    if (mode == "walking" || mode == "running" || mode == "standing" || mode == "birth") {
        absPos.z() = Environment::getInstance().getHeight(absPos.x(), absPos.y());
        
        if (loc->type() == "jetty") {
          float jetty_z = loc->getAbsPos().z();
          if (absPos.z() < jetty_z) absPos.z() = jetty_z;
        }
      } else if (mode == "floating") {
        // Do nothing, use server Z
      } else if (mode == "swimming") {
        // Do nothing, use server Z
      }
    } else if (loc->hasProperty("terrain")) {  
        absPos.z() = Environment::getInstance().getHeight(absPos.x(), absPos.y());
    }

  return absPos;
}

void WorldEntity::displayInfo() {
  Log::writeLog(std::string("Entity ID: ") + getID(), Log::LOG_DEFAULT);
  System::instance()->pushMessage(std::string(getName()) + std::string(" - id: ") + std::string(getID()), CONSOLE_MESSAGE | SCREEN_MESSAGE);
  Log::writeLog(std::string("Entity Name: ") + getName(), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Type: ") + type(), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Parent Type: ") + parent(), Log::LOG_DEFAULT);
  WFMath::Vector<3> pos = getInterpolatedPos();
  Log::writeLog(std::string("X: ") + string_fmt(pos.x()) + std::string(" Y: ") + string_fmt(pos.y()) + std::string(" Z: ") + string_fmt(pos.z()), Log::LOG_DEFAULT);
  
  WFMath::Point<3> abspos = getAbsPos();
  Log::writeLog(std::string("ABS - X: ") + string_fmt(abspos.x()) + std::string(" Y: ") + string_fmt(abspos.y()) + std::string(" Z: ") + string_fmt(abspos.z()), Log::LOG_DEFAULT);
  Eris::Entity *e = getContainer();
  Log::writeLog(std::string("Parent: ") + ((e == NULL) ? ("NULL") : (e->getID())), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Num Children: ") + string_fmt(getNumMembers()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Has Bounding Box: ") + string_fmt(hasBBox()), Log::LOG_DEFAULT);
  WFMath::AxisBox<3> bbox = getBBox();
  Log::writeLog(std::string("Ux: ") + string_fmt(bbox.lowCorner().x()) + std::string(" Uy: ") + string_fmt(bbox.lowCorner().y()) + std::string(" Uz: ") + string_fmt(bbox.lowCorner().z()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Vx: ") + string_fmt(bbox.highCorner().x()) + std::string(" Vy: ") + string_fmt(bbox.highCorner().y()) + std::string(" Vz: ") + string_fmt(bbox.highCorner().z()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Visibility: ") + ((isVisible()) ? ("true") : ("false")), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Stamp: ") + string_fmt(getStamp()), Log::LOG_DEFAULT);
  if (hasProperty("mass")) {

    double mass = getProperty("mass").asNum();
  Log::writeLog(std::string("Mass: ") + string_fmt(mass), Log::LOG_DEFAULT);
  System::instance()->pushMessage(std::string(getName()) + std::string(" - Mass: ") + std::string(string_fmt(mass)), CONSOLE_MESSAGE | SCREEN_MESSAGE);
  }
  if (hasProperty(MODE)) {
    std::string mode = getProperty(MODE).asString();
    Log::writeLog(std::string("Mode: ") + mode, Log::LOG_DEFAULT);
}
}

std::string WorldEntity::type() {
  Eris::TypeInfo *ti = getType();
  if (ti) return ti->getName();
  else return "";
}

std::string WorldEntity::parent() {
  Eris::TypeInfo *ti = getType();
  if (ti) {
    if (ti->getParentsAsSet().size() > 0)
      return *ti->getParentsAsSet().begin();
  }
  return "";
//  if (_parents.size() > 0) return *_parents.begin();
//  else return "";
}

void WorldEntity::checkActions() {
  return;
  ObjectHandler *object_handler = System::instance()->getObjectHandler();

  // TODO possibility to link into action handler
  
  if (hasProperty(ACTION)) {
    std::string action = getProperty(ACTION).asString();
    if (debug) std::cout << "Action: " << action << std::endl;
    if (action != last_action) {
      ObjectRecord *record = NULL;
      if (object_handler) record = object_handler->getObjectRecord(getID());
      if (record) record->action(action);
      last_action = action;
    }
  } else {
    last_action == "";
  }
	 
  if (hasProperty(MODE)) {
    std::string mode = getProperty(MODE).asString();
    if (debug) std::cout << "Mode: " << mode << std::endl;
    if (mode != last_mode) {
      ObjectRecord *record = NULL;
      if (object_handler) record = object_handler->getObjectRecord(getID());
      if (record) record->action(mode);
      last_mode = mode;
    }
  } else {
    last_mode == "";
  }
}

void WorldEntity::sigChanged(const Eris::StringSet &ss) {
  ObjectHandler *object_handler = System::instance()->getObjectHandler();
  for (Eris::StringSet::const_iterator I = ss.begin(); I != ss.end(); ++I) {
    std::string str = *I;
    if (debug) std::cout << "Changed - " << str << std::endl;
    if (str == MODE) {
      const std::string mode = getProperty(MODE).asString();
      static ActionHandler *ac = System::instance()->getActionHandler();
      ac->handleAction(mode + "_" + type(), NULL);
      if (debug) std::cout << "Mode: " << mode << std::endl;
      if (mode != last_mode) {
        ObjectRecord *record = NULL;
        if (object_handler) record = object_handler->getObjectRecord(getID());
        if (record) record->action(mode);
        last_mode = mode;
      }
    } else if (str == ACTION) {
      const std::string action = getProperty(ACTION).asString();
      static ActionHandler *ac = System::instance()->getActionHandler();
      ac->handleAction(action + "_" + type(), NULL);
      std::cout << "Action: " << action << std::endl;
      if (action != last_action) {
        ObjectRecord *record = NULL;
        if (object_handler) record = object_handler->getObjectRecord(getID());
        if (record) record->action(action);
        last_action = action;
      }
    } else if (str == GUISE) {
      Atlas::Message::Element::MapType mt = getProperty(GUISE).asMap();
      ObjectRecord *record = NULL;
      if (object_handler) record = object_handler->getObjectRecord(getID());
      if (record) record->setAppearance(mt);
    } else if (str == "bbox") {
std::cout << "Changing Height;" << std::endl;
      float height = fabs(getBBox().highCorner().z() - getBBox().lowCorner().z());
      ObjectRecord *record = NULL;
      if (object_handler) record = object_handler->getObjectRecord(getID());
      if (record) record->setHeight(height);
    }
  }
}

void WorldEntity::rotateBBox(const WFMath::Quaternion &q)
{
  m_orientBBox = OrientBBox(getBBox());
  m_orientBBox.rotate(q);
}
  
WFMath::Vector<3> WorldEntity::getInterpolatedPos() const
{
    double dt = (System::instance()->getTime() - m_lastMoveTime) / 1000.0;
    return (getPosition() + (getVelocity() * dt)) - WFMath::Point<3>(0,0,0);
}

} /* namespace Sear */
