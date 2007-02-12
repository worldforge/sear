// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: WorldEntity.cpp,v 1.94 2007-02-12 21:44:00 simon Exp $

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

static const std::string ATTR_action = "action";
static const std::string ATTR_description = "description";
static const std::string ATTR_guide = "guise";
static const std::string ATTR_mass = "mass";
static const std::string ATTR_mode = "mode";
static const std::string ATTR_outfit = "outfit";
static const std::string ATTR_right_hand_wield = "right_hand_wield";
static const std::string ATTR_say = "say";
static const std::string ATTR_status = "status";
static const std::string ATTR_terrain = "terrain";

static const std::string MODE_floating = "floating";
static const std::string MODE_fixed = "fixed";
static const std::string MODE_swimming = "swimming";

static const std::string TYPE_jetty = "jetty";

static const WFMath::Point<3> point_zero = WFMath::Point<3>(0.0f,0.0f,0.0f);

namespace Sear {

WorldEntity::WorldEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view):
   Eris::Entity(id, ty, view),
   m_status(1.0),
   m_screenCoordRequest(0),
   m_has_local_orient(false),
   m_has_local_pos(false),
   m_selected(false),
   m_fading(false),
   m_fade_in(false),
   m_fade(1.0f)
{
  Acted.connect(sigc::mem_fun(this, &WorldEntity::onAction));
  LocationChanged.connect(sigc::mem_fun(this, &WorldEntity::locationChanged));
  ChildAdded.connect(sigc::mem_fun(this, &WorldEntity::onChildEntityAdded));
  ChildRemoved.connect(sigc::mem_fun(this, &WorldEntity::onChildEntityRemoved));
  BeingDeleted.connect(sigc::mem_fun(this, &WorldEntity::onBeingDeleted));

  // Get type and parent type name
  Eris::TypeInfo *ti = getType();
  if (ti) { 
    m_type = ti->getName();
    if (ti->getParents().size() > 0) {
      ti = *ti->getParents().begin();
      m_parent_type = ti->getName();
    }
  }
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
  if (!talkArg->hasAttr(ATTR_say)) {
    printf("Error: Talk but no 'say'\n");
    return;
  }
  const std::string &msg = talkArg->getAttr(ATTR_say).asString();

  Log::writeLog(getId() + std::string(": ") + msg, Log::LOG_DEFAULT);
  System::instance()->pushMessage(getName()+ ": " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);

  if (m_messages.size() >= MAX_MESSAGES) m_messages.erase(m_messages.begin());
  m_messages.push_back(message(msg, System::instance()->getTime() + message_life));  
  Eris::Entity::onTalk(talk);
}

void WorldEntity::onImaginary(const Atlas::Objects::Root &imaginaryArg)
{
    Atlas::Message::Element attr;
    if (imaginaryArg->copyAttr(ATTR_description, attr) != 0 || !attr.isString()) {
        return;
    }
    System::instance()->pushMessage(getName()+" " + attr.String(), CONSOLE_MESSAGE | SCREEN_MESSAGE);
}

//const WFMath::Quaternion &WorldEntity::getAbsOrient() {
void WorldEntity::updateAbsOrient() {

  WorldEntity *loc = dynamic_cast<WorldEntity*>(getLocation());
  if (!loc) {
    m_abs_orient = getEntityOrientation(); // nothing below makes sense for the world
    return;
  }
  
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

  m_abs_orient = orient;
}

//const WFMath::Point<3> &WorldEntity::getAbsPos() {
void WorldEntity::updateAbsPosition() {
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
      if (loc->hasAttr(ATTR_mode)) {
        const std::string &mode = loc->valueOfAttr(ATTR_mode).asString();
        if (mode == MODE_swimming) {
          // Make sure height is > terrain height
          needTerrainHeight = clampHeight = true;
        } else if (mode == MODE_floating) {
          // Do nothing at all.
          // Should we do something here?
          // E.g. set to water height?
        } else if (mode == MODE_fixed) {
          // Do nothing at all.
        } else {
          needTerrainHeight = setHeight = true;
        }
      } else {
        needTerrainHeight = setHeight = true;
      }
      if (!loc_loc->hasAttr(ATTR_terrain)) {
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
      if (loc_loc->type() == TYPE_jetty) {
        // We want to make sure the height is jetty height, unless the terrain 
        // is poking through, then we want to use terrain height
        // This is assuming that the jetty object is directly in the world.

        // Getty jetty position
        const WFMath::Point<3> &p = loc_loc->getAbsPos();
        // Calculate the position of the current entity in terms of the jetty.
        const WFMath::Point<3> &p2 = p + (lpos - point_zero).rotate(loc_loc->getEntityOrientation());
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

  m_abs_position = absPos;
}

void WorldEntity::displayInfo() {
  std::string msg_name = getName() + " - id: " + getId();
  System::instance()->pushMessage(msg_name, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  if (debug) {
    printf("Entity ID: %s\n", getId().c_str());
    printf("Entity Name: %s\n", getName().c_str());
    printf("Type: %s\n", type().c_str());
    printf("Parent Type: %s\n", parent().c_str());
  
    const WFMath::Point<3> &pos = getEntityPosition();
    printf("Pos - (%f, %f, %f)\n", pos.x(), pos.y(), pos.z());
    const WFMath::Point<3> &abspos = getAbsPos();

    printf("ABS Pos (%f, %f, %f)\n", abspos.x(), abspos.y(), abspos.z());
    Eris::Entity *e = getLocation();
    printf("Parent: %s\n", (e == NULL) ? ("NULL") : (e->getId().c_str()));

    WorldEntity *loc =  dynamic_cast<WorldEntity*>(getLocation());
    if (loc) {
      const WFMath::Point<3> &labspos = loc->getAbsPos();
      printf("LABS - (%f, %f, %f)\n", labspos.x(), labspos.y(), labspos.z());
    }

    printf("Num Children: %d\n", numContained());
    printf("Has Bounding Box: %d\n", hasBBox());
    const WFMath::AxisBox<3> &bbox = getBBox();

    printf("U (%f, %f, %f)\n", bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z());
    printf("V (%f, %f, %f)\n", bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z());
    printf("Visibility: %d\n", isVisible());
    printf("Stamp: %f\n", getStamp());
    if (hasAttr(ATTR_mode)) {
      const std::string &mode = valueOfAttr(ATTR_mode).asString();
      printf("Mode: %s\n", mode.c_str());
    }
  }
  if (hasAttr(ATTR_mass)) {
    double mass = valueOfAttr(ATTR_mass).asNum();
    if (debug) printf("Mass: %f\n", mass);
    const std::string &msg_mass = getName() + " - Mass: " + string_fmt(mass);
    System::instance()->pushMessage(msg_mass, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  }
}

void WorldEntity::onAttrChanged(const std::string& str, const Atlas::Message::Element& v) {
  if (str == ATTR_mode) {
    const std::string &mode = v.asString();
    static ActionHandler *ac = System::instance()->getActionHandler();
    assert(ac);
    ac->handleAction(mode + "_" + type(), NULL);
    if (mode != m_last_mode) {
      SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this);
      if (record) record->animate(mode);
      m_last_mode = mode;
    }
  } else if (str == ATTR_right_hand_wield) {
    const std::string &id = v.asString();
    if (id.empty()) {
     m_attached.erase(ATTR_right_hand_wield);
    } else {
      WorldEntity* attach = dynamic_cast<WorldEntity*>(getView()->getEntity(id));
      if (attach) {
        m_attached[ATTR_right_hand_wield] = attach;
      } else {
        Eris::View::EntitySightSlot ess(sigc::bind( 
          sigc::mem_fun(this, &WorldEntity::onSightAttached),
          str));
        getView()->notifyWhenEntitySeen(id, ess);
      }
    }
  } else if (str == ATTR_status) {
    m_status = v.asNum();
  } else if (str == ATTR_outfit) {
    SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this); 
    if (!record) return;
    record->clearOutfit();

    if (v.isMap() == true) {
      const Atlas::Message::MapType &mt = v.asMap();
      Atlas::Message::MapType::const_iterator I = mt.begin();
      Atlas::Message::MapType::const_iterator Iend = mt.end();
      while (I != Iend) {
        if (I->second.isString()) {
          const std::string &where = I->first;
          const std::string &id = I->second.asString();
          WorldEntity *we = dynamic_cast<WorldEntity*>(getView()->getEntity(id));
          if (we) {
            record->entityWorn(where, we); 
          } else {
            Eris::View::EntitySightSlot ess(sigc::bind( 
            sigc::mem_fun(this, &WorldEntity::onSightOutfit),
            where));
            getView()->notifyWhenEntitySeen(id, ess);
          }
        }
        ++I;
      }
    }    
  }
}

void WorldEntity::onSightOutfit(Eris::Entity *ent, std::string where) {
  SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this); 
  WorldEntity *we = dynamic_cast<WorldEntity*>(ent);
  if (we) record->entityWorn(where, we);
}

void WorldEntity::onSightAttached(Eris::Entity* ent, const std::string slot)
{
  if (debug) printf("Slot is %s\n", slot.c_str());
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

  const std::string &a = *I;

  static ActionHandler *ac = System::instance()->getActionHandler();
  ac->handleAction(a + "_" + type(), NULL);

  SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this);
  if (record) record->action(a);
}

void WorldEntity::locationChanged(Eris::Entity *loc) {
  resetLocalPO();
  updateAbsOrient();
  updateAbsPosition();
}

void WorldEntity::onChildEntityAdded(Eris::Entity *e) {
//  SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this);
//  if (record) record->entityWorn(dynamic_cast<WorldEntity*>(e));
}

void WorldEntity::onChildEntityRemoved(Eris::Entity *e) {
//  SPtr<ObjectRecord> record = ModelSystem::getInstance().getObjectRecord(this);
//  if (record) record->entityRemoved(dynamic_cast<WorldEntity*>(e));
}

void WorldEntity::onBeingDeleted() {
  // Detach callbacks..
  // This may detach more than we really want. E.g. other onDeleted callback
  // handlers.
  notify_callbacks();
}

void WorldEntity::updateFade(float f) {
  if (!m_fading) return;
  if (m_fade_in) {
    m_fade += f;
  } else {
    m_fade -= f;
  }
  if (m_fade < 0.0f) {
    m_fade = 0.0;
    m_fading = false;
  } else if (m_fade > 1.0f) {
    m_fade = 1.0f;
    m_fading = false;
  }
}


static void dumpElement(const std::string &prefix, const std::string &name, const Atlas::Message::Element &e) {

  if (e.isMap()) {
    printf("%s%s: Dumping Map\n", prefix.c_str(), name.c_str());
    Atlas::Message::MapType::const_iterator itr = e.asMap().begin();
    Atlas::Message::MapType::const_iterator end = e.asMap().end();
    for (; itr != end; ++itr) {
      dumpElement(prefix + "  ", itr->first, itr->second);
    }
    printf("%sFinished Dumping Map\n", prefix.c_str());
  } else {
    if (e.isString()) printf("%s%s: %s\n", prefix.c_str(), name.c_str(), e.asString().c_str());
    if (e.isNum()) printf("%s%s: %f\n", prefix.c_str(), name.c_str(), e.asNum());
  }
}

void WorldEntity::dumpAttributes() const {
  printf("Dumping attributes for entity %s (%s)\n", getId().c_str(), getName().c_str());
  const Eris::Entity::AttrMap &attribs = getAttributes();

  Eris::Entity::AttrMap::const_iterator itr = attribs.begin();
  Eris::Entity::AttrMap::const_iterator end = attribs.end();
  for (; itr != end; ++itr) {
    dumpElement("  ",itr->first, itr->second);
  }

}


} /* namespace Sear */
