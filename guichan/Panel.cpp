// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/Panel.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"

#include "src/System.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

Panel::Panel() : gcn::Window()
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  gcn::Box * hbox = new gcn::HBox(6);

  m_inventoryButton = new gcn::Button("Login");
  m_buttonListener = new ActionListenerSigC;
  SigC::Slot0<void> s1 = SigC::bind<panel_button>(SigC::slot(*this, &Panel::actionPressed), PANEL_INVENTORY);
  SigC::Slot1<void, std::string> s2 = SigC::hide<std::string>(s1);
  m_buttonListener->Action.connect(s2);
  m_inventoryButton->addActionListener(m_buttonListener);
  hbox->pack(m_inventoryButton);

  setContent(hbox);

  resizeToContent();
}

Panel::~Panel()
{
}

void Panel::actionPressed(panel_button a)
{

}

} // namespace Sear
