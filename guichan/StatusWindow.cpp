// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/StatusWindow.h"

#include "guichan/box.hpp"
#include "guichan/bar.hpp"

#include "src/System.h"
#include "src/Character.h"

#include <Eris/Entity.h>

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>
#include <cassert>

namespace Sear {

StatusWindow::StatusWindow(Eris::Entity * e) : m_entity(e)
{
  assert(e != 0);

  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  // setTitleBarHeight(0);
  setMovable(false);

  setCaption(e->getName());

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
  if (m_entity) {
    if (m_entity->hasAttr("status")) {
      Atlas::Message::Element status_attr = m_entity->valueOfAttr("status");
      if (status_attr.isNum()) {
        m_healthBar->setValue(status_attr.asNum());
      }
    }
    if (m_entity->hasAttr("stamina")) {
      Atlas::Message::Element stamina_attr = m_entity->valueOfAttr("stamina");
      if (stamina_attr.isNum()) {
        m_staminaBar->setValue(stamina_attr.asNum());
      }
    }
    if (m_entity->hasAttr("mana")) {
      Atlas::Message::Element mana_attr = m_entity->valueOfAttr("mana");
      if (mana_attr.isNum()) {
        m_manaBar->setValue(mana_attr.asNum());
      }
    }
  }
  gcn::Window::logic();
}

} // namespace Sear
