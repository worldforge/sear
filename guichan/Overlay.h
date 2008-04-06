// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_OVERLAY_H
#define SEAR_GUICHAN_OVERLAY_H

#include <guichan.hpp>

#include <Eris/EntityRef.h>

#include <Atlas/Objects/RootOperation.h>

#include <sigc++/object.h>

#include <common/SPtr.h>

namespace gcn {
}

namespace Eris {
  class Entity;
}

namespace Sear {

class RootWidget;
class SpeechBubble;
class StatusWindow;
class TaskWindow;
class WorldEntity;

class Overlay : virtual public sigc::trackable {
  friend class SPtr<Overlay>;
public:
  typedef std::map<Eris::EntityRef, SPtr<SpeechBubble> > BubbleMap;
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  Overlay();
  ~Overlay();

  RootWidget * m_top;
  BubbleMap m_bubbles;
  Eris::EntityRef m_selection;

  SPtr<StatusWindow> m_selfStatus;
  SPtr<StatusWindow> m_selectionStatus;
  SPtr<TaskWindow> m_selfTask;

  static SPtr<Overlay> m_instance;
public:
  static Overlay * instance() {
    if (!m_instance) {
      m_instance = SPtr<Overlay>(new Overlay());
    }
    return m_instance.get();
  }

  void heard(Eris::Entity *, const Atlas::Objects::Operation::RootOperation &);

  void logic(RootWidget *);

  void contextCreated();
  void contextDestroyed(bool check);

};

} // namespace Sear

#endif // SEAR_GUICHAN_OVERLAY_H
