// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch

#ifndef SEAR_GUICHAN_TASKWINDOW_H
#define SEAR_GUICHAN_TASKWINDOW_H

#include <Eris/EntityRef.h>

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include "common/SPtr.h"

// #include <map>
// #include <string>

namespace gcn {

class Box;
class Bar;

} // namespace gcn

namespace Sear {

class TaskWindow : // virtual public SigC::Object,
                     public gcn::Window {
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  gcn::Box * m_vbox;
  gcn::Bar * m_progressBar;

  Eris::EntityRef m_entity;
public:
  explicit TaskWindow(Eris::Entity *);
  virtual ~TaskWindow();

  virtual void logic();
};

} // namespace Sear

#endif // SEAR_GUICHAN_TASKWINDOW_H
