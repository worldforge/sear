// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _WORLDENTITY_H_
#define _WORLDENTITY_H_ 1

#include <string>
#include <list>
#include <wfmath/quaternion.h>
#include <wfmath/point.h>
#include <Eris/Entity.h>
#include <Eris/World.h>

#include "ObjectLoader.h"

namespace Sear {

typedef std::pair<std::string, unsigned int> message;
static const unsigned int WORLD_ENTITY_MESSAGE_LIFE = 5000;
static const unsigned int WORLD_ENTITY_STRING_SIZE = 40;

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
  void renderMessages();
  void displayInfo();

  std::string type();
  std::string parent();

  ObjectProperties *getObjectProperties() { return _op; }
  void setObjectProperties(ObjectProperties *op) { _op = op; }

protected:
  typedef std::pair<std::string, unsigned int> screenMessage;
  unsigned int time;
  WFMath::Quaternion abs_orient;
  WFMath::Point<3> abs_pos;
  std::list<message> messages;
  ObjectProperties *_op;

  static int message_life;
  static int string_size;
};

} /* namespace Sear */
#endif /* _WORLDENTITY_H_ */
