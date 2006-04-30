// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_OVERLAY_H
#define SEAR_GUICHAN_OVERLAY_H

#include <guichan.hpp>

#include <Eris/EntityRef.h>

#include <Atlas/Objects/RootOperation.h>

#include <sigc++/object.h>

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

class Overlay : virtual public SigC::Object {
public:
  typedef std::map<WorldEntity *, SpeechBubble *> BubbleMap;
protected:
  Overlay();
  ~Overlay();

  RootWidget * m_top;
  BubbleMap m_bubbles;
  Eris::EntityRef m_selection;

  StatusWindow * m_selfStatus;
  StatusWindow * m_selectionStatus;
  TaskWindow * m_selfTask;

  static Overlay * m_instance;
public:
  static Overlay * instance() {
    if (m_instance == 0) {
      m_instance = new Overlay();
    }
    return m_instance;
  }

  void heard(Eris::Entity *, const Atlas::Objects::Operation::RootOperation &);

  void logic(RootWidget *);
};

} // namespace Sear

#endif // SEAR_GUICHAN_OVERLAY_H
