// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: WorldEntity.h,v 1.19 2005-02-18 16:39:06 simon Exp $

#ifndef SEAR_WORLDENTITY_H
#define SEAR_WORLDENTITY_H 1

#include <string>
#include <list>
#include <wfmath/quaternion.h>
#include <wfmath/point.h>
#include <Eris/Entity.h>
#include <Eris/Types.h>
#include "common/types.h"

//#include "ObjectLoader.h"
namespace Eris {
  class View;
}

namespace Sear {

typedef std::pair<std::string, unsigned int> message;

class WorldEntity : public Eris::Entity {
public:
  WorldEntity::WorldEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view);
  ~WorldEntity();
  
  void onMove();
  void handleTalk(const std::string &);

//  const WFMath::Quaternion getAbsOrient();
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
  /** Calculate the current position of this entity relative to it's
  container (location). This is based on the last pos set by the server,
  and a linear interpolation of the last set velocity. */
//  WFMath::Vector<3> getInterpolatedPos() const;

  typedef std::pair<std::string, unsigned int> screenMessage;

  std::list<message> messages;
  //ObjectProperties *_op;

  static const int message_life = 5000;
  static const int string_size = 40;
  std::string last_action;
  std::string last_mode;
  
  void sigChanged(const Eris::StringSet &ss);
friend class Character;

  OrientBBox m_orientBBox;
//  unsigned int m_lastMoveTime;
};

} /* namespace Sear */
#endif /* SEAR_WORLDENTITY_H */
