// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/StatusWindow.h"

#include "guichan/box.hpp"
#include "guichan/bar.hpp"

#include "src/System.h"
#include "src/Character.h"

#include <Eris/Avatar.h>
#include <Eris/Entity.h>

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

static std::string PANEL_OPEN = "panel_open";
static std::string PANEL_CLOSE = "panel_close";
static std::string PANEL_TOGGLE = "panel_toggle";

StatusWindow::StatusWindow()
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setTitleBarHeight(0);
  setMovable(false);

  // setOpaque(true);

  m_vbox = new gcn::VBox(2);

  m_healthBar = new gcn::Bar;
  m_healthBar->setWidth(100);
  m_healthBar->setHeight(6);
  m_healthBar->setValue(0.3);
  m_healthBar->setForegroundColor(gcn::Color(0xff0000));
  m_healthBar->setBaseColor(gcn::Color(0,0,0,25));
  m_vbox->pack(m_healthBar);

  m_staminaBar = new gcn::Bar;
  m_staminaBar->setWidth(100);
  m_staminaBar->setHeight(6);
  m_staminaBar->setValue(0.6);
  m_staminaBar->setForegroundColor(gcn::Color(0xeeeeee));
  m_staminaBar->setBaseColor(gcn::Color(0,0,0,25));
  m_vbox->pack(m_staminaBar);

  m_manaBar = new gcn::Bar;
  m_manaBar->setWidth(100);
  m_manaBar->setHeight(6);
  m_manaBar->setValue(0.9);
  m_manaBar->setForegroundColor(gcn::Color(0x0000ff));
  m_manaBar->setBaseColor(gcn::Color(0,0,0,25));
  m_vbox->pack(m_manaBar);

  setContent(m_vbox);
  resizeToContent();
}

StatusWindow::~StatusWindow()
{
}

void StatusWindow::logic()
{
  System * s = System::instance();
  if (s != 0) {
    Character * c = s->getCharacter();
    if (c != 0) {
      Eris::Avatar * av = c->getAvatar();
      if (av != 0) {
        Eris::Entity * ent = av->getEntity();
        if (ent != 0) {
          if (ent->hasAttr("status")) {
            Atlas::Message::Element status_attr = ent->valueOfAttr("status");
            if (status_attr.isNum()) {
              m_healthBar->setValue(status_attr.asNum());
            }
          }
        }
      }
    }
  }
  gcn::Window::logic();
}

} // namespace Sear
