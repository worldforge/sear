// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: WorldEntity.cpp,v 1.58 2005-06-02 11:18:15 simon Exp $

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
#include "loaders/ObjectHandler.h"
#include "loaders/ObjectRecord.h"
#include "loaders/ModelSystem.h"
#include "loaders/Model.h"
#include "renderers/Render.h"
#include "renderers/RenderSystem.h"
#include "WorldEntity.h"
#include "ActionHandler.h"

#include "environment/Environment.h"
#include <set>
#include <sigc++/bind.h>

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
	
WorldEntity::WorldEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view):
   Eris::Entity(id, ty, view),
   messages(std::list<message>())
{
}

void WorldEntity::onMove() {
  rotateBBox(getOrientation());
}

void WorldEntity::onTalk(const Atlas::Objects::Root &talkArgs) {
  if (!talkArgs->hasAttr("say")) {
    printf("Error: Talk but no 'say'\n");
    return;
  }
  std::string msg = talkArgs->getAttr("say").asString();

  Log::writeLog(getId() + std::string(": ") + msg, Log::LOG_DEFAULT);	
  System::instance()->pushMessage(getName()+ ": " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);

  if (messages.size() >= MAX_MESSAGES) messages.erase(messages.begin());
  messages.push_back(message(msg, System::instance()->getTime() + message_life));  
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
  Render *renderer = RenderSystem::getInstance().getRenderer();
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
    WorldEntity *loc =  dynamic_cast<WorldEntity*>(getLocation());
    if (loc) parentOrient = loc->getAbsOrient();
    return parentOrient / getOrientation();
}

const WFMath::Point<3> WorldEntity::getAbsPos() {
 // This function is still required to adjust the Z position of the entity

  WorldEntity* loc = static_cast<WorldEntity*>(getLocation());
  if (!loc) return getPredictedPos(); // nothing below makes sense for the world.
//    
  WFMath::Point<3> absPos(
    loc->getPredictedPos().x() + getPredictedPos().x(),
    loc->getPredictedPos().y() + getPredictedPos().y(),
    loc->getPredictedPos().z() + getPredictedPos().z());

  float terrainOffset = 0.0f;
  if (loc->hasAttr("terrain")) {  
    terrainOffset = Environment::getInstance().getHeight(absPos.x(), absPos.y());
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
      if (absPos.z() < terrainOffset) absPos.z() = terrainOffset;
    } else {
      // Assume clamped to terrain
      absPos.z() = terrainOffset;
    }
  }  
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
//  WFMath::Vector<3> pos = getInterpolatedPos();
//  Log::writeLog(std::string("X: ") + string_fmt(pos.x()) + std::string(" Y: ") + string_fmt(pos.y()) + std::string(" Z: ") + string_fmt(pos.z()), Log::LOG_DEFAULT);
  
//  WFMath::Point<3> abspos = getAbsPos();
//  Log::writeLog(std::string("ABS - X: ") + string_fmt(abspos.x()) + std::string(" Y: ") + string_fmt(abspos.y()) + std::string(" Z: ") + string_fmt(abspos.z()), Log::LOG_DEFAULT);
  Eris::Entity *e = getLocation();
  Log::writeLog(std::string("Parent: ") + ((e == NULL) ? ("NULL") : (e->getId())), Log::LOG_DEFAULT);
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
    if (ti->getParents().size() > 0) {
      ti = *ti->getParents().begin();
      return ti->getName();
    }
  }
  return "";
}

void WorldEntity::onAttrChanged(const std::string& str, const Atlas::Message::Element& v) {
  ObjectHandler *object_handler = ModelSystem::getInstance().getObjectHandler();

    if (str == MODE) {
      const std::string mode = v.asString();
      static ActionHandler *ac = System::instance()->getActionHandler();
      ac->handleAction(mode + "_" + type(), NULL);
      if (mode != last_mode) {
        ObjectRecord *record = NULL;
        if (object_handler) record = object_handler->getObjectRecord(getId());
        if (record) record->action(mode);
        last_mode = mode;
      }
    } else if (str == ACTION) {
      const std::string action = v.asString();
      static ActionHandler *ac = System::instance()->getActionHandler();
      ac->handleAction(action + "_" + type(), NULL);
      if (action != last_action) {
        ObjectRecord *record = NULL;
        if (object_handler) record = object_handler->getObjectRecord(getId());
        if (record) record->action(action);
        last_action = action;
      }
    } else if (str == GUISE) {
      const Atlas::Message::MapType& mt(v.asMap());
      ObjectRecord *record = NULL;
      if (object_handler) record = object_handler->getObjectRecord(getId());
      if (record) record->setAppearance(mt);
    } else if (str == "bbox") {
      float height = fabs(getBBox().highCorner().z() - getBBox().lowCorner().z());
      ObjectRecord *record = NULL;
      if (object_handler) record = object_handler->getObjectRecord(getId());
      if (record) record->setHeight(height);
    } else if (str == "right_hand_wield") {
        std::string id = v.asString();
        if (id.empty()) {
            m_attached.erase("right_hand_wield");
        } else {
            WorldEntity* attach = dynamic_cast<WorldEntity*>(getView()->getEntity(id));
            if (attach) {
                m_attached["right_hand_wield"] = attach;
            } else {
                Eris::View::EntitySightSlot ess(SigC::bind( 
                    SigC::slot(*this, &WorldEntity::onSightAttached),
                    str));
                getView()->notifyWhenEntitySeen(id, ess);
            }
        }
    }
}

void WorldEntity::onSightAttached(Eris::Entity* ent, const std::string slot)
{
    m_attached[slot] =  dynamic_cast<WorldEntity*>(ent);
}

void WorldEntity::rotateBBox(const WFMath::Quaternion &q)
{
  m_orientBBox = OrientBBox(getBBox());
  m_orientBBox.rotate(q);
}

} /* namespace Sear */
