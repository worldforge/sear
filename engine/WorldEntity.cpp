// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: WorldEntity.cpp,v 1.1.2.2 2003-01-05 18:18:27 simon Exp $

#include "System.h"
#include <wfmath/axisbox.h>
#include <Eris/TypeInfo.h>

#include "script/ScriptEngine.h"

#include "WorldEntity.h"

#include <set>

static const bool debug = true;

namespace Sear {

static const std::string ACTION = "action";
static const std::string MODE = "mode";
	
WorldEntity::WorldEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world):
   Eris::Entity(ge, world),
   time(0),
   abs_orient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f)),
   abs_pos(WFMath::Point<3>(0.0f, 0.0f, 0.0f))
{

}

WorldEntity::~WorldEntity() {
}

void WorldEntity::handleMove() {
  SetVelocity();
  WorldEntity *we = (WorldEntity*)getContainer();
  if (we != NULL) {
    translateAbsPos(we->getAbsPos());
    rotateAbsOrient(we->getAbsOrient());
  } else {
    translateAbsPos(WFMath::Point<3>(0.0f, 0.0f, 0.0f));
    rotateAbsOrient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
  }
  ScriptEngine::instance()->entityEvent(getID(),"onMove", "position");
}

void WorldEntity::handleTalk(const std::string &msg) {
  if (debug) std::cout << _id << ": " << msg << std::endl;	
  ScriptEngine::instance()->entityEvent(getID(),"onTalk", msg);
}

void WorldEntity::setProperty(const std::string &s, const Atlas::Message::Object &val) {
  Eris::Entity::setProperty(s, val);
  ScriptEngine::instance()->entityEvent(getID(),"onProperty", s);

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
  for (unsigned int i = 0; i < getNumMembers(); ++i)
    ((WorldEntity*)getMember(i))->translateAbsPos(child_pos);
}

void WorldEntity::rotateAbsOrient(WFMath::Quaternion q) {
  abs_orient = q;
  WFMath::Quaternion child_orient = q / getOrientation();
  for (unsigned int i = 0; i < getNumMembers(); ++i)
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

std::string WorldEntity::getAProperty(const std::string &property) {
  if (hasProperty(property)) {
    return getProperty(property).AsString();
  } else return "";
}

void WorldEntity::registerCommands(lua_State *L) {
  lua_register(L, "entity_get_entity", &WorldEntity::l_get_entity);
  lua_register(L, "entity_get_name", &WorldEntity::l_get_name);
  lua_register(L, "entity_get_id", &WorldEntity::l_get_id);
  lua_register(L, "entity_get_type", &WorldEntity::l_get_type);
  lua_register(L, "entity_get_parent", &WorldEntity::l_get_parent);
  lua_register(L, "entity_get_property", &WorldEntity::l_get_property);
//  lua_register(L, "get_pos", &WorldEntity::l_get_entity);
//  lua_register(L, "get_abs_pos", &WorldEntity::l_get_entity);
//  lua_register(L, "get_orient", &WorldEntity::l_get_entity);
//  lua_register(L, "get_abs_orient", &WorldEntity::l_get_entity);
}

int WorldEntity::l_get_entity(lua_State *L) {
  if (debug) std::cout << "l_get_entity" << std::endl;
  int top = lua_gettop(L);
  if (top != 1) {
    std::cerr << "Not enough args for l_get_entity" << std::endl;
    lua_pushusertag(L, (int)0, LUA_ANYTAG);
    return 1;
  }
  std::string id = lua_tostring(L, 1);
  if (debug) std::cout << "Looking up entity - " << id << std::endl;
  if (Eris::World::Instance()) {
    WorldEntity *we = static_cast<WorldEntity*>(Eris::World::Instance()->lookup(id));
    if (!we) std::cerr << "WorldEntity is NULL" << std::endl;
    lua_pushusertag(L, reinterpret_cast<void*>(we), LUA_ANYTAG);
  } else {
    std::cerr << "World is NULL" << std::endl;
    lua_pushusertag(L, (int)0, LUA_ANYTAG);
  }
  return 1;
}

int WorldEntity::l_get_name(lua_State *L) {
  int top = lua_gettop(L);
  if (top != 1) {
    std::cerr << "Not enough args for l_get_name" << std::endl;
    lua_pushstring(L, "");
    return 1;
  }
  WorldEntity *we = reinterpret_cast<WorldEntity*>(lua_touserdata(L, 1));
  if (we) {
    lua_pushstring(L, we->getName().c_str());
  } else {
    std::cerr << "WorldEntity is NULL" << std::endl;
    lua_pushstring(L, "");
  }
  return 1;
}

int WorldEntity::l_get_id(lua_State *L) {
  int top = lua_gettop(L);
  if (top != 1) {
    std::cerr << "Not enough args for l_get_id" << std::endl;
    lua_pushstring(L, "");
    return 1;
  }
  WorldEntity *we = reinterpret_cast<WorldEntity*>(lua_touserdata(L, 1));
  if (we) {
    lua_pushstring(L, we->getID().c_str());
  } else {
    std::cerr << "WorldEntity is NULL" << std::endl;
    lua_pushstring(L, "");
  }
  return 1;
}


int WorldEntity::l_get_type(lua_State *L) {
  int top = lua_gettop(L);
  if (top != 1) {
    std::cerr << "Not enough args for l_get_type" << std::endl;
    lua_pushstring(L, "");
    return 1;
  }
  WorldEntity *we = reinterpret_cast<WorldEntity*>(lua_touserdata(L, 1));
  if (we) {
    lua_pushstring(L, we->type().c_str());
  } else {
    std::cerr << "WorldEntity is NULL" << std::endl;
    lua_pushstring(L, "");
  }
  return 1;
}

int WorldEntity::l_get_parent(lua_State *L) {
  int top = lua_gettop(L);
  if (top != 1) {
    std::cerr << "Not enough args for l_get_parent" << std::endl;
    lua_pushstring(L, "");
    return 1;
  }
  WorldEntity *we = reinterpret_cast<WorldEntity*>(lua_touserdata(L, 1));
  if (we) {
    lua_pushstring(L, we->parent().c_str());
  } else {
    std::cerr << "WorldEntity is NULL" << std::endl;
    lua_pushstring(L, "");
  }
  return 1;
}

int WorldEntity::l_get_property(lua_State *L) {
  int top = lua_gettop(L);
  if (top != 2) {
    std::cerr << "Not enough args for l_get_id" << std::endl;
    lua_pushstring(L, "");
    return 1;
  }
  WorldEntity *we = reinterpret_cast<WorldEntity*>(lua_touserdata(L, 1));
  std::string property = lua_tostring(L, 2);
  if (we) {
    if (we->hasProperty(property)) {
      lua_pushstring(L, we->getProperty(property).AsString().c_str());
    } else {
      std::cerr << "Unknown Property" << std::endl;
      lua_pushstring(L, "");
    }
  } else {
    std::cerr << "WorldEntity is NULL" << std::endl;
    lua_pushstring(L, "");
  }
  return 1;
}



} /* namespace Sear */
