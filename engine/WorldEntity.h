// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: WorldEntity.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_WORLDENTITY_H
#define SEAR_WORLDENTITY_H 1
extern "C" {
  #include <lua.h>
}

#include <string>
#include <list>
#include <wfmath/quaternion.h>
#include <wfmath/point.h>
#include <Eris/Entity.h>
#include <Eris/World.h>

//#include "ObjectLoader.h"

namespace Sear {

typedef std::pair<std::string, unsigned int> message;

class WorldEntity : public Eris::Entity {
public:
 WorldEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world);
  ~WorldEntity();
  void SetVelocity();
  WFMath::Point<3> GetPos(); 
  void handleMove();
  void handleTalk(const std::string &);

  void translateAbsPos(WFMath::Point<3>);
  void rotateAbsOrient(WFMath::Quaternion);
  WFMath::Quaternion getAbsOrient();
  WFMath::Point<3> getAbsPos();

  std::string type();
  std::string parent();
  std::string getAProperty(const std::string &property);
 
  static void registerCommands(lua_State *L);
  
protected:
  unsigned int time;
  WFMath::Quaternion abs_orient;
  WFMath::Point<3> abs_pos;

  static int l_get_entity(lua_State *L);
  static int l_get_name(lua_State *L);
  static int l_get_id(lua_State *L);
  static int l_get_type(lua_State *L);
  static int l_get_parent(lua_State *L);
  static int l_get_property(lua_State *L);
  
};

} /* namespace Sear */
#endif /* SEAR_WORLDENTITY_H */
