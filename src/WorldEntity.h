// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _WORLDENTITY_H_
#define _WORLDENTITY_H_ 1

#include <wfmath/quaternion.h>
#include <wfmath/point.h>
#include <Eris/Entity.h>

#include "debug.h"

#define WORLD_ENTITY_MESSAGE_LIFE (5000)
#define WORLD_ENTITY_STRING_SIZE  (40)

typedef std::pair<std::string, unsigned int> message;

class WorldEntity : public Eris::Entity {
public:

 WorldEntity(const Atlas::Objects::Entity::GameEntity &ge):
   Eris::Entity(ge),
   time(0),
   abs_orient(WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f)),
   abs_pos(WFMath::Point<3>(0.0f, 0.0f, 0.0f)),
   messages(std::list<message>())
   {}
  ~WorldEntity() {}
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

protected:
  typedef std::pair<std::string, unsigned int> screenMessage;
  unsigned int time;
  WFMath::Quaternion abs_orient;
  WFMath::Point<3> abs_pos;
  std::list<message> messages;

  static int message_life;
  static int string_size;
};

#endif /* _WORLDENTITY_H_ */
