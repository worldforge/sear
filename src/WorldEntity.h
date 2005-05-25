// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: WorldEntity.h,v 1.25 2005-05-25 12:42:51 jmt Exp $

#ifndef SEAR_WORLDENTITY_H
#define SEAR_WORLDENTITY_H 1

#include <string>
#include <list>
#include <Atlas/Objects/Operation.h>

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
  virtual ~WorldEntity() {}
  
  void onMove();
  void onTalk(const Atlas::Objects::Root &talkArgs);

  const WFMath::Quaternion getAbsOrient();
  const WFMath::Point<3> getAbsPos();

  const OrientBBox &getOrientBBox() const { return m_orientBBox; }
  bool hasMessages() const { return !messages.empty(); }
  void renderMessages();
  void displayInfo();

  std::string type();
  std::string parent();

  void rotateBBox(const WFMath::Quaternion &q);  

  // make this a map of vectors in the future
  typedef std::map<std::string, WorldEntity*> AttachmentMap;
  
  const AttachmentMap& getAttachments() const
  { return m_attached; }
protected:

  typedef std::pair<std::string, unsigned int> screenMessage;

  std::list<message> messages;

  static const int message_life = 5000;
  static const int string_size = 40;
  std::string last_action;
  std::string last_mode;
  
  void onAttrChanged(const std::string& attr, const Atlas::Message::Element& v);
  
  friend class Character;

  OrientBBox m_orientBBox;
  
  AttachmentMap m_attached;
};

} /* namespace Sear */

#endif /* SEAR_WORLDENTITY_H */
