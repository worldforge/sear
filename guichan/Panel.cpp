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

  setTitleBarHeight(0);
  setMovable(false);

  // setOpaque(true);

  gcn::Box * hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &Panel::actionPressed));

  m_optionsButton = new gcn::Button("Options");
  m_optionsButton->setEventId("options");
  m_optionsButton->addActionListener(m_buttonListener);
  hbox->pack(m_optionsButton);

  m_inventoryButton = new gcn::Button("Inventory");
  m_inventoryButton->setEventId("inventory");
  m_inventoryButton->addActionListener(m_buttonListener);
  hbox->pack(m_inventoryButton);

  hbox->pack(new gcn::Button("Test"));
  hbox->pack(new gcn::Button("Test"));
  hbox->pack(new gcn::Button("Test"));
  hbox->pack(new gcn::Button("Test"));

  setContent(hbox);

  resizeToContent();
}

Panel::~Panel()
{
}

void Panel::actionPressed(std::string event)
{
  if (event == "options") {
    std::cout << "Open options window" << std::endl << std::flush;
  } else if (event == "inventory") {
    std::cout << "Open inventory window" << std::endl << std::flush;
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
