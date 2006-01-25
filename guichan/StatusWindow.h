// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_STATUSWINDOW_H
#define SEAR_GUICHAN_STATUSWINDOW_H

#include <Eris/EntityRef.h>

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

// #include <map>
// #include <string>

namespace gcn {

class Box;
class Bar;

} // namespace gcn

namespace Sear {

class StatusWindow : // virtual public SigC::Object,
                     public gcn::Window {
protected:
  gcn::Box * m_vbox;
  gcn::Bar * m_healthBar;
  gcn::Bar * m_staminaBar;
  gcn::Bar * m_manaBar;

  Eris::EntityRef m_entity;
public:
  explicit StatusWindow(Eris::Entity *);
  virtual ~StatusWindow();

  virtual void logic();
};

} // namespace Sear

#endif // SEAR_GUICHAN_STATUSWINDOW_H
