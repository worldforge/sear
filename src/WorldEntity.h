// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: WorldEntity.h,v 1.16 2004-06-20 18:43:35 simon Exp $

#ifndef SEAR_WORLDENTITY_H
#define SEAR_WORLDENTITY_H 1

#include <string>
#include <list>
#include <wfmath/quaternion.h>
#include <wfmath/point.h>
#include <Eris/Entity.h>
#include <Eris/Types.h>
#include <Eris/World.h>
#include "common/types.h"

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

  void translateAbsPos(const WFMath::Point<3> &);
  void rotateAbsOrient(const WFMath::Quaternion &);
  const WFMath::Quaternion getAbsOrient();
  const WFMath::Point<3> getAbsPos();

  const OrientBBox &getOrientBBox() const { return m_orientBBox; }
  bool hasMessages();
  void renderMessages();
  void displayInfo();

  std::string type();
  std::string parent();

//  ObjectProperties *getObjectProperties() { return _op; }
 // void setObjectProperties(ObjectProperties *op) { _op = op; }
  void checkActions();
  void rotateBBox(const WFMath::Quaternion &q);  
protected:
  typedef std::pair<std::string, unsigned int> screenMessage;
  unsigned int time;
  WFMath::Quaternion abs_orient;
  WFMath::Point<3> abs_pos;
  std::list<message> messages;
  //ObjectProperties *_op;

  static const int message_life = 5000;
  static const int string_size = 40;
  std::string last_action;
  std::string last_mode;
  
  void sigChanged(const Eris::StringSet &ss);
friend class Character;

  OrientBBox m_orientBBox;
};

} /* namespace Sear */
#endif /* SEAR_WORLDENTITY_H */
