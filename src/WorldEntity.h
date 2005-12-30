// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: WorldEntity.h,v 1.33 2005-12-30 18:11:44 alriddoch Exp $

#ifndef SEAR_WORLDENTITY_H
#define SEAR_WORLDENTITY_H 1

#include <string>
#include <list>
#include <Atlas/Objects/Operation.h>

#include <wfmath/quaternion.h>
#include <wfmath/point.h>
#include <Eris/Entity.h>
#include <Eris/EntityRef.h>
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
  WorldEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view);
  virtual ~WorldEntity() {}
  
  void onMove();
  void onTalk(const Atlas::Objects::Root &talkArgs);
  void onImaginary(const Atlas::Objects::Root &imaginaryArg);

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
  typedef std::map<std::string, Eris::EntityRef> AttachmentMap;
  
  const AttachmentMap& getAttachments() const
  { return m_attached; }
  
  double getStatus() const
  { return m_status; }
  
protected:

  typedef std::pair<std::string, unsigned int> screenMessage;

  std::list<message> messages;

  static const int message_life = 5000;
  static const int string_size = 40;
  std::string last_action;
  std::string last_mode;
  
  void onAttrChanged(const std::string& attr, const Atlas::Message::Element& v);
  
  void onSightAttached(Eris::Entity* ent, const std::string slot);
  void onAction(const Atlas::Objects::Operation::RootOperation &action);
  
  friend class Character;

  OrientBBox m_orientBBox;
  AttachmentMap m_attached;
  double m_status;
};

} /* namespace Sear */

#endif /* SEAR_WORLDENTITY_H */
