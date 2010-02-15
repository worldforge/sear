// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall, University of Southampton

#ifndef SEAR_WORLDENTITY_H
#define SEAR_WORLDENTITY_H 1

#include <string>
#include <list>
#include <Atlas/Objects/ObjectsFwd.h>

#include <wfmath/quaternion.h>
#include <wfmath/point.h>
#include <Eris/ViewEntity.h>
#include <Eris/EntityRef.h>
#include <Eris/Types.h>
#include "common/types.h"

namespace Eris {
  class View;
}

namespace Atlas {
  namespace Message {
    class Element;
  }
}

namespace Sear {

typedef std::pair<std::string, unsigned int> message;

class WorldEntity : public Eris::ViewEntity {
public:
  WorldEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view);
  virtual ~WorldEntity() {}
  
  void onMove();
  void onTalk(const Atlas::Objects::Operation::RootOperation &talk);
  void onImaginary(const Atlas::Objects::Root &imaginaryArg);

  const WFMath::Quaternion &getAbsOrient() const { return m_abs_orient; }
  const WFMath::Point<3> &getAbsPos() const { return m_abs_position; }

  const OrientBBox &getOrientBBox() const { return m_orientBBox; }

  bool hasMessages() const { return !m_messages.empty(); }

  void displayInfo();

  const std::string &type() const { return m_type; }
  const std::string &parent() const { return m_parent_type; }

  void rotateBBox(const WFMath::Quaternion &q);  

  // make this a map of vectors in the future
  typedef std::map<std::string, Eris::EntityRef> AttachmentMap;
  
  const AttachmentMap& getAttachments() const { return m_attached; }
  
  double getStatus() const { return m_status; }

  void requestScreenCoords() { ++m_screenCoordRequest; }  
  void releaseScreenCoords()  { --m_screenCoordRequest; }
  int screenCoordsRequest() const { return m_screenCoordRequest; }

  int & screenX() { return m_screenX; }
  int & screenY() { return m_screenY; }

  void setLocalPos(const WFMath::Point<3> &pos) {
    m_local_pos = pos;
    m_has_local_pos = true;
  }

  void setLocalOrient(const WFMath::Quaternion &orient) {
    m_local_orient = orient;
    m_has_local_orient = true;
  }

  void resetLocalPO() {
    m_has_local_orient = false;
    m_has_local_pos = false;
  }

  bool isSelectedEntity() const { return m_selected; }
  void setIsSelected(bool b) {  m_selected = b; }

  bool isFading() const { return m_fading; }
  float getFade() const { return m_fade; }

  void updateFade(float f);

  void startFadeIn() {
    m_fading = true;
    m_fade = 0.0f;
    m_fade_in = true;
  }

  void startFadeOut() {
    m_fading = true;
    m_fade = 1.0f;
    m_fade_in = false;
  }

  void dumpAttributes() const;

  // Call these functions to update the position and orientation
  // These must be called once per frame, and whenever the local overrides
  // change.
  void updateAbsPosition();
  void updateAbsOrient();

  /**
   * Return composite string of the view and entity id.
   */ 
  const std::string &getViewId() const { return m_view_id; }

protected:
  WFMath::Point<3> getEntityPosition() const {
    if (m_has_local_pos) return m_local_pos;
    else return getPredictedPos();
  }

  const WFMath::Quaternion &getEntityOrientation() const {
    if (m_has_local_orient) return m_local_orient;
    else return getOrientation();
  }

  typedef std::pair<std::string, unsigned int> screenMessage;

  std::vector<message> m_messages;

  static const int message_life = 5000;
  static const int string_size = 40;
  std::string m_last_action;
  std::string m_last_mode;
  std::string m_type, m_parent_type;
  
  void onAttrChanged(const std::string& attr, const Atlas::Message::Element& v);
  void locationChanged(Eris::Entity *loc);
  void onSightAttached(Eris::Entity* ent, const std::string slot);
  void onSightOutfit(Eris::Entity* ent, const std::string where);
  void onAction(const Atlas::Objects::Operation::RootOperation &action);
  void onChildEntityAdded(Eris::Entity*);
  void onChildEntityRemoved(Eris::Entity*);
  void onBeingDeleted();
  
  friend class Character;

  OrientBBox m_orientBBox;
  AttachmentMap m_attached;
  double m_status;

  int m_screenCoordRequest;
  int m_screenX, m_screenY;

  bool m_has_local_orient, m_has_local_pos;
  WFMath::Quaternion m_local_orient, m_abs_orient;
  WFMath::Point<3> m_local_pos, m_abs_position;

  bool m_selected;

  bool m_fading, m_fade_in;
  float m_fade;
  const std::string m_view_id;
};

} /* namespace Sear */

#endif /* SEAR_WORLDENTITY_H */
