// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include <wfmath/axisbox.h>
#include <Eris/TypeInfo.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "Console.h"
#include "Event.h"
#include "EventHandler.h"
#include "Graphics.h"
#include "ModelHandler.h"
#include "Model.h"
#include "Render.h"
#include "System.h"
#include "WorldEntity.h"

#include <set>

namespace Sear {

int WorldEntity::string_size = WORLD_ENTITY_STRING_SIZE;
int WorldEntity::message_life = WORLD_ENTITY_MESSAGE_LIFE;

void WorldEntity::handleMove() {
  SetVelocity();
  WorldEntity *we = (WorldEntity*)getContainer();
  static ModelHandler *model_handler = System::instance()->getModelHandler();
  if (we != NULL) {
    translateAbsPos(we->getAbsPos());
    rotateAbsOrient(we->getAbsOrient());
  } else {
    translateAbsPos(WFMath::Point<3>(0.0f, 0.0f, 0.0f));
    rotateAbsOrient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
  }
  Model *model = NULL;
  if (model_handler) model = model_handler->getModel(this);
  if (model) {
    if (SQR(_velocity.x()) + SQR(_velocity.y()) + SQR(_velocity.z()) >= 7.0f) model->action("run");
    else model->action("walk");
  }
}

void WorldEntity::handleTalk(const std::string &msg) {
  Log::writeLog(_id + std::string(": ") + msg, Log::LOG_DEFAULT);	
  System::instance()->pushMessage(_name + ": " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);

  if (messages.size() >= MAX_MESSAGES) messages.erase(messages.begin());
  messages.push_back(message(msg, System::instance()->getTime() + message_life));  
}

void WorldEntity::renderMessages() {
  if (messages.empty()) return;
  //Convert messages into char arrays up x characters long
  std::list<std::string> mesgs = std::list<std::string>();
  std::list<screenMessage>::reverse_iterator I;
  for (I = messages.rbegin(); I != messages.rend(); I++) {
    const std::string str = (const std::string)((*I).first);
    std::list<std::string> message_list = std::list<std::string>();
    int pos = 0;
    while (true) {
      std::string str1;
      try {
        str1 = str.substr(pos, pos + string_size); // Get first 40 characters
      } catch (...) { // less than 40 characters left
	try {
          str1 = str.substr(pos);
	} catch (...) { // no characters left
          str1 = "";
	}
      }
      if (str1.empty()) break; // No more text, end loop
      // Add text to mesgs list
      message_list.push_back(str1);
      pos += string_size;
    }
    std::list<std::string>::reverse_iterator K;
    for (K = message_list.rbegin(); K != message_list.rend(); K++) {
      mesgs.push_back(*K);
    }
     
  }
  // Render text strings
  static Render *renderer = System::instance()->getGraphics()->getRender();
  std::list<std::string>::iterator J;
  for (J = mesgs.begin(); J != mesgs.end(); J++) {	  
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

void WorldEntity::SetVelocity() {
  time = System::instance()->getTime();
}

WFMath::Point<3> WorldEntity::GetPos() {
  unsigned int ui = System::instance()->getTime() - time;
  float tme = (float)ui / 1000.0f; // Convert into seconds

  float newx = (tme * _velocity.x()) + _position.x();
  float newy = (tme * _velocity.y()) + _position.y();
  float newz = (tme * _velocity.z()) + _position.z();
  return WFMath::Point<3>(newx, newy, newz); 
}

void WorldEntity::translateAbsPos(WFMath::Point<3> p) {
  abs_pos = p;
  WFMath::Point<3> pos = _position;
  WFMath::Point<3> child_pos = WFMath::Point<3>(p.x() + pos.x(), p.y() + pos.y(), p.z() + pos.z());
  for (unsigned int i = 0; i < getNumMembers(); i++)
    ((WorldEntity*)getMember(i))->translateAbsPos(child_pos);
}

void WorldEntity::rotateAbsOrient(WFMath::Quaternion q) {
  abs_orient = q;
  WFMath::Quaternion child_orient = q / getOrientation();
  for (unsigned int i = 0; i < getNumMembers(); i++)
    ((WorldEntity*)getMember(i))->rotateAbsOrient(child_orient);
}

WFMath::Quaternion WorldEntity::getAbsOrient() {
  WFMath::Quaternion new_orient =  abs_orient / getOrientation();
  return new_orient;
}

WFMath::Point<3> WorldEntity::getAbsPos() {
  WFMath::Point<3> pos = GetPos();
  WFMath::Point<3> new_pos = WFMath::Point<3>(abs_pos.x() + pos.x(), abs_pos.y() + pos.y(), abs_pos.z() + pos.z());
  return new_pos;
}

void WorldEntity::displayInfo() {
  Log::writeLog(std::string("Entity ID: ") + getID(), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Entity Name: ") + getName(), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Type: ") + type(), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Parent Type: ") + parent(), Log::LOG_DEFAULT);
  WFMath::Point<3> pos = GetPos();
  Log::writeLog(std::string("X: ") + string_fmt(pos.x()) + std::string(" Y: ") + string_fmt(pos.y()) + std::string(" Z: ") + string_fmt(pos.z()), Log::LOG_DEFAULT);
  Eris::Entity *e = getContainer();
  Log::writeLog(std::string("Parent: ") + ((e == NULL) ? ("NULL") : (e->getID())), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Num Children: ") + string_fmt(getNumMembers()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Has Bounding Box: ") + string_fmt(hasBBox()), Log::LOG_DEFAULT);
  WFMath::AxisBox<3> bbox = getBBox();
  Log::writeLog(std::string("Ux: ") + string_fmt(bbox.lowCorner().x()) + std::string(" Uy: ") + string_fmt(bbox.lowCorner().y()) + std::string(" Uz: ") + string_fmt(bbox.lowCorner().z()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Vx: ") + string_fmt(bbox.highCorner().x()) + std::string(" Vy: ") + string_fmt(bbox.highCorner().y()) + std::string(" Vz: ") + string_fmt(bbox.highCorner().z()), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Visibility: ") + ((isVisible()) ? ("true") : ("false")), Log::LOG_DEFAULT);
  Log::writeLog(std::string("Stamp: ") + string_fmt(getStamp()), Log::LOG_DEFAULT);
}

std::string WorldEntity::type() {
  Eris::TypeInfo *ti = getType();
  if (ti) return ti->getName();
  else return "";
}

std::string WorldEntity::parent() {
  Eris::TypeInfo *ti = getType();
  if (ti) {
	  return *ti->getParentsAsSet().begin();
  }
  return "";
}

} /* namespace Sear */
