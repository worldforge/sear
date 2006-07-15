// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: WorldEntity.cpp,v 1.85 2006-07-15 13:17:48 simon Exp $

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

#include <Atlas/Objects/Operation.h>

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

static const WFMath::Point<3> point_zero = WFMath::Point<3>(0.0f,0.0f,0.0f);

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
  if (!orient.isValid()) {
    fprintf(stderr, "Warning: invalid orientation detected. (ID: %s Name: %s)\n", getId().c_str(), getName().c_str());
    orient.identity();
  }
  while (loc != 0 ) {
    WFMath::Quaternion lorient = loc->getEntityOrientation();

    if (!lorient.isValid()) { // TODO: Replace with assert once eris is fixed
      fprintf(stderr, "Warning: invalid orientation detected for parent object. (ID: %s Name: %s)\n", getId().c_str(), getName().c_str());
      lorient.identity();
    }

    orient *= lorient;

    loc = dynamic_cast<WorldEntity*>(loc->getLocation());
  }

  if (!orient.isValid()) {
    fprintf(stderr, "Warning: invalid orientation detected for abs orient. (ID: %s Name: %s)\n", getId().c_str(), getName().c_str());
    orient.identity();
  }

  return orient;
}

const WFMath::Point<3> WorldEntity::getAbsPos() {
  WFMath::Point<3> absPos(point_zero);
  WorldEntity *loc = this;
  
  while (loc != NULL) {
    // Cache parent location
    WorldEntity *loc_loc = dynamic_cast<WorldEntity*>(loc->getLocation());

    WFMath::Point<3> lpos = loc->getEntityPosition();

    if (!lpos.isValid()) { // TODO: Replace with assert once eris is fixed
      lpos = point_zero;
    }
    WFMath::Quaternion lorient = loc->getEntityOrientation();
    if (!lorient.isValid()) { // TODO: Replace with assert once eris is fixed
      lorient.identity();
    }

    // Silly WFMath syntax makes it hard to rotate a point
    WFMath::Vector<3> newVec((absPos-point_zero).rotate(lorient));

    // Do lots of hackish stuff to set the Z value
    if (loc_loc) {
      bool needTerrainHeight = false;
      bool setHeight = false;
      bool clampHeight = false;
      // Get the terrain height for x,y pos, but don't set it yet as the mode
      // needs to have a say first.
      if (loc->hasAttr(MODE)) {
        std::string mode = loc->valueOfAttr(MODE).asString();
        if (mode == "swimming") {
          // Make sure height is > terrain height
          needTerrainHeight = clampHeight = true;
        } else if (mode == "floating") {
          // Do nothing at all.
          // Should we do something here?
          // E.g. set to water height?
        } else if (mode == "fixed") {
          // Do nothing at all.
        } else {
          needTerrainHeight = setHeight = true;
        }
      } else {
        needTerrainHeight = setHeight = true;
      }
      if (!loc_loc->hasAttr("terrain")) {
        needTerrainHeight = false;
      }
      if (needTerrainHeight) {
        float h = Environment::getInstance().getHeight(lpos.x(), lpos.y());
        if (setHeight) {
          lpos.z() = h;
        } else if (clampHeight) {
          if (lpos.z() < h) lpos.z() = h;
        }
      }

      // Hack for clamping entity height to jetty objects.
      // This should be handled better, perhaps by checking an attribute.
      if (loc_loc->type() == "jetty") {
        // We want to make sure the height is jetty height, unless the terrain 
        // is poking through, then we want to use terrain height
        // This is assuming that the jetty object is directly in the world.

        // Getty jetty position
        WFMath::Point<3> p = loc_loc->getAbsPos();
        // Calculate the position of the current entity in terms of the jetty.
        WFMath::Point<3> p2 = p + (lpos - point_zero).rotate(loc_loc->getEntityOrientation());
        // Get the predicted height for the current entity. This is assuming
        // that the jetty is contained by an entity with a terrain attribute.
        // Perhaps we could recurse through parents until we find the terrain 
        // entity. That is kinda duplicating this function to make this function
        // work.
        float h1 = Environment::getInstance().getHeight(p2.x(), p2.y());
        // If the current entity is higher than the jetty, set Z pos to the 
        // difference, else set to 0. (Assuming that the jetty platform is at 0
        // on the model!
        lpos.z() = (h1 > p.z()) ? (h1 - p.z()) : (0.0);
      }
    }
   
    absPos = lpos + newVec;

    loc = loc_loc;
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
