// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: WorldEntity.cpp,v 1.80 2006-02-20 20:20:02 simon Exp $

/*
 TODO
  * getAbsPos and getAbsOrient should cache the value unless it has changed
  * Easy for none moving chars, but moving chars will need resetting once per
  * frame.
*/

//#include <set>
#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <Atlas/Message/Element.h>

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>

#include <Eris/Types.h>
#include <Eris/TypeInfo.h>
#include <Eris/View.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "System.h"
#include "Console.h"
#include "loaders/ObjectRecord.h"
#include "loaders/ModelSystem.h"
#include "WorldEntity.h"
#include "ActionHandler.h"

#include "environment/Environment.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

static const std::string ACTION = "action";
static const std::string MODE = "mode";
static const std::string GUISE = "guise";
	
WorldEntity::WorldEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view):
   Eris::Entity(id, ty, view),
   m_status(1.0),
   m_screenCoordRequest(0),
   m_has_local_orient(false),
   m_has_local_pos(false)
{
  Acted.connect(SigC::slot(*this, &WorldEntity::onAction));
  LocationChanged.connect(sigc::mem_fun(this, &WorldEntity::locationChanged));
}

void WorldEntity::onMove() {
  rotateBBox(getEntityOrientation());
}

void WorldEntity::onTalk(const Atlas::Objects::Operation::RootOperation &talk)
{
  const std::vector<Atlas::Objects::Root> & talkArgs = talk->getArgs();
  if (talkArgs.empty())
  {
    return;
  }
  const Atlas::Objects::Root & talkArg = talkArgs.front();
  if (!talkArg->hasAttr("say")) {
    printf("Error: Talk but no 'say'\n");
    return;
  }
  std::string msg = talkArg->getAttr("say").asString();

  Log::writeLog(getId() + std::string(": ") + msg, Log::LOG_DEFAULT);	
  System::instance()->pushMessage(getName()+ ": " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);

  if (messages.size() >= MAX_MESSAGES) messages.erase(messages.begin());
  messages.push_back(message(msg, System::instance()->getTime() + message_life));  
  Eris::Entity::onTalk(talk);
}

void WorldEntity::onImaginary(const Atlas::Objects::Root &imaginaryArg)
{
    Atlas::Message::Element attr;
    if (imaginaryArg->copyAttr("description", attr) != 0 || !attr.isString()) {
        return;
    }
    System::instance()->pushMessage(getName()+" " + attr.String(), CONSOLE_MESSAGE | SCREEN_MESSAGE);
}

const WFMath::Quaternion WorldEntity::getAbsOrient() {

  WorldEntity *loc = dynamic_cast<WorldEntity*>(getLocation());
  if (!loc) return getEntityOrientation(); // nothing below makes sense for the world
  
  WFMath::Quaternion orient = getEntityOrientation();
  while (loc != 0 ) {
    WFMath::Quaternion lorient = loc->getEntityOrientation();

    if (!lorient.isValid()) { // TODO: Replace with assert once eris is fixed
      lorient.identity();
    }
    orient *= lorient;

    loc = dynamic_cast<WorldEntity*>(loc->getLocation());
  }
  return orient;
}

const WFMath::Point<3> WorldEntity::getAbsPos() {
  // Get parent entity for additional positional information
  WorldEntity *loc = dynamic_cast<WorldEntity*>(getLocation());
  if (!loc) return getEntityPosition(); // nothing below makes sense for the world

  // Cache predicted pos of entity
  WFMath::Point<3> predicted = getEntityPosition();
  //assert(predicted.isValid());
  if (!predicted.isValid()) { // TODO: Replace with assert once eris is fixed
    predicted = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
  }

  WFMath::Point<3> absPos = predicted;
  while (loc != NULL) {
    WFMath::Point<3> lpos = loc->getEntityPosition();

    if (!lpos.isValid()) { // TODO: Replace with assert once eris is fixed
      lpos = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    }
    WFMath::Quaternion lorient = loc->getEntityOrientation();
    if (!lorient.isValid()) { // TODO: Replace with assert once eris is fixed
      lorient.identity();
    }

    // Silly WFMath syntax makes it hard to rotate a point
    absPos = lpos + ((absPos - WFMath::Point<3>(0,0,0)).rotate(lorient));

    loc = dynamic_cast<WorldEntity*>(loc->getLocation());
  }
 
  // reset loc back to real parent
  loc = dynamic_cast<WorldEntity*>(getLocation());

  float terrainHeight = 0.0f;
  bool hasHeight = false;
  if (loc->hasAttr("terrain")) {  
    terrainHeight = Environment::getInstance().getHeight(absPos.x(), absPos.y()); 
    hasHeight = true;
  }
    
  // Set Z coord to terrain height if required
  if (hasAttr(MODE)) {
    std::string mode = valueOfAttr(MODE).asString();
    if (mode == "floating") {
      // Set to water level
      absPos.z() = 0.0f;
    } else if (mode == "swimming") {
      // Clamp between sea level and terrain height.
      // If there is a dispute, then place object on top of terrain.
      if (absPos.z() > 0.0f) absPos.z() = 0.0f;
      if (absPos.z() < terrainHeight) absPos.z() = terrainHeight;
    } else if (mode == "fixed") {
      // Do predited entity pos as absolute position
//      absPos.x() = pred.x();
//      absPos.y() = pred.y();
//      absPos.z() = pred.z();
    } else {
      // Assume clamped to terrain
      if (hasHeight) absPos.z() = terrainHeight;
    }
  } else {
    // Assume clamped to terrain
    if (hasHeight) absPos.z() = terrainHeight;
  }
  // Hack for clamping entity height to jetty objects.
  // This should be handled better.  
  if (loc->type() == "jetty") {
    float jetty_z = loc->getAbsPos().z();
    if (absPos.z() < jetty_z) absPos.z() = jetty_z;
  }

  return absPos;
}

void WorldEntity::displayInfo() {
  Log::writeLog(std::string("Entity ID: ") + getId(), Log::LOG_DEFAULT);
  System::instance()->pushMessage(std::string(getName()) + std::string(" - id: ") + std::string(getId()), CONSOLE_MESSAGE | SCREEN_MESSAGE);
  Log::writeLog(std::string("Entity Name: ") + getName(), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Type: ") + type(), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Parent Type: ") + parent(), Log::LOG_DEFAULT);
  
  WFMath::Point<3> pos = getEntityPosition();
  Log::writeLog(std::string("Pos - X: ") + string_fmt(pos.x()) + std::string(" Y: ") + string_fmt(pos.y()) + std::string(" Z: ") + string_fmt(pos.z()), Log::LOG_DEFAULT);
  WFMath::Point<3> abspos = getAbsPos();
  Log::writeLog(std::string("ABS - X: ") + string_fmt(abspos.x()) + std::string(" Y: ") + string_fmt(abspos.y()) + std::string(" Z: ") + string_fmt(abspos.z()), Log::LOG_DEFAULT);
  Eris::Entity *e = getLocation();
  Log::writeLog(std::string("Parent: ") + ((e == NULL) ? ("NULL") : (e->getId())), Log::LOG_DEFAULT);

    WorldEntity *loc =  dynamic_cast<WorldEntity*>(getLocation());
if (loc) {
  WFMath::Point<3> labspos = loc->getAbsPos();
  Log::writeLog(std::string("LABS - X: ") + string_fmt(labspos.x()) + std::string(" Y: ") + string_fmt(labspos.y()) + std::string(" Z: ") + string_fmt(labspos.z()), Log::LOG_DEFAULT);
}

  Log::writeLog(std::string("Num Children: ") + string_fmt(numContained()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Has Bounding Box: ") + string_fmt(hasBBox()), Log::LOG_DEFAULT);
  WFMath::AxisBox<3> bbox = getBBox();
  Log::writeLog(std::string("Ux: ") + string_fmt(bbox.lowCorner().x()) + std::string(" Uy: ") + string_fmt(bbox.lowCorner().y()) + std::string(" Uz: ") + string_fmt(bbox.lowCorner().z()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Vx: ") + string_fmt(bbox.highCorner().x()) + std::string(" Vy: ") + string_fmt(bbox.highCorner().y()) + std::string(" Vz: ") + string_fmt(bbox.highCorner().z()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Visibility: ") + ((isVisible()) ? ("true") : ("false")), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Stamp: ") + string_fmt(getStamp()), Log::LOG_DEFAULT);
  if (hasAttr("mass")) {

    double mass = valueOfAttr("mass").asNum();
    Log::writeLog(std::string("Mass: ") + string_fmt(mass), Log::LOG_DEFAULT);
    System::instance()->pushMessage(std::string(getName()) + std::string(" - Mass: ") + std::string(string_fmt(mass)), CONSOLE_MESSAGE | SCREEN_MESSAGE);
  }
  if (hasAttr(MODE)) {
    std::string mode = valueOfAttr(MODE).asString();
    if (debug) Log::writeLog(std::string("Mode: ") + mode, Log::LOG_DEFAULT);
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
    if (ti->getParents().size() > 0) {
      ti = *ti->getParents().begin();
      return ti->getName();
    }
  }
  return "";
}

void WorldEntity::onAttrChanged(const std::string& str, const Atlas::Message::Element& v) {
  if (str == MODE) {
    const std::string mode = v.asString();
    static ActionHandler *ac = System::instance()->getActionHandler();
    assert(ac);
    ac->handleAction(mode + "_" + type(), NULL);
    if (mode != last_mode) {
      SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this);
      if (record) record->animate(mode);
      last_mode = mode;
    }
  } else if (str == GUISE) {
    const Atlas::Message::MapType& mt(v.asMap());
    SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this);
    if (record) record->setAppearance(mt);
  } else if (str == "right_hand_wield") {
    std::string id = v.asString();
    if (id.empty()) {
     m_attached.erase("right_hand_wield");
    } else {
      WorldEntity* attach = dynamic_cast<WorldEntity*>(getView()->getEntity(id));
      if (attach) {
        m_attached["right_hand_wield"] = attach;
      } else {
        Eris::View::EntitySightSlot ess(sigc::bind( 
          SigC::slot(*this, &WorldEntity::onSightAttached),
          str));
        getView()->notifyWhenEntitySeen(id, ess);
      }
    }
  } else if (str == "status") {
    m_status = v.asNum();
  }
}

void WorldEntity::onSightAttached(Eris::Entity* ent, const std::string slot)
{
    m_attached[slot] = dynamic_cast<WorldEntity*>(ent);
}

void WorldEntity::rotateBBox(const WFMath::Quaternion &q)
{
  m_orientBBox = OrientBBox(getBBox());
  m_orientBBox.rotate(q);
}

void WorldEntity::onAction(const Atlas::Objects::Operation::RootOperation &action) {
  const std::list<std::string> &p = action->getParents();
  std::list<std::string>::const_iterator I = p.begin();

  if (I == p.end()) return;

  std::string a = *I;

  static ActionHandler *ac = System::instance()->getActionHandler();
  ac->handleAction(a + "_" + type(), NULL);

  SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this);
  if (record) record->action(a);
}

void WorldEntity::locationChanged(Eris::Entity *loc) {
  resetLocalPO();
}

} /* namespace Sear */
