// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/Inventory.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"

#include "src/System.h"
#include "src/Character.h"

#include <Eris/Avatar.h>

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

class InventoryListAdaptor : public gcn::ListModel
{
public:
  virtual int getNumberOfElements()
  {
    Character * chr = System::instance()->getCharacter();
    if (chr == 0) { return 0; }
    Eris::Avatar * av = chr->getAvatar();
    if (av == 0) { return 0; }
    return av->getEntity()->numContained();
  }

  virtual std::string getElementAt(int i)
  {
    Character * chr = System::instance()->getCharacter();
    if (chr == 0) { return ""; }
    Eris::Avatar * av = chr->getAvatar();
    if (av == 0) { return ""; }
    if (i >= av->getEntity()->numContained()) { return ""; }
    return av->getEntity()->getContained(i)->getName();
  }
};


Inventory::Inventory() : gcn::Window()
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  gcn::ListBox * servers = new gcn::ListBox(new InventoryListAdaptor);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(servers,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  scroll_area->setWidth(120);
  scroll_area->setHeight(100);
  scroll_area->setBorderSize(1);
  vbox->pack(scroll_area);

  gcn::Box * hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &Inventory::actionPressed));

  m_wieldButton = new gcn::Button("Wield");
  m_wieldButton->setEventId("wield");
  m_wieldButton->addActionListener(m_buttonListener);
  hbox->pack(m_wieldButton);

  m_giveButton = new gcn::Button("Give");
  m_giveButton->setEventId("give");
  m_giveButton->addActionListener(m_buttonListener);
  hbox->pack(m_giveButton);

  m_dropButton = new gcn::Button("Drop");
  m_dropButton->setEventId("drop");
  m_dropButton->addActionListener(m_buttonListener);
  hbox->pack(m_dropButton);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();
}

Inventory::~Inventory()
{
}

void Inventory::actionPressed(std::string event)
{
  if (event == "wield") {
    std::string cmd("/wield ");
    // cmd += m_serverField->getText();
    // System::instance()->runCommand(cmd);
  } else if (event == "give") {
    std::string cmd("/give ");
    // std::cout << "Close window" << std::endl << std::flush;
  } else if (event == "drop") {
    std::string cmd("/drop ");
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
