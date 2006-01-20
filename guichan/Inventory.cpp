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


Inventory::Inventory() : gcn::Window("Inventory")
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  m_inventory = new InventoryListAdaptor;
  m_items = new gcn::ListBox(m_inventory);
  m_items->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_items,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  scroll_area->setWidth(120);
  scroll_area->setHeight(100);
  scroll_area->setBorderSize(1);
  scroll_area->setFocusable(false);
  vbox->pack(scroll_area);

  gcn::Box * hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &Inventory::actionPressed));

  gcn::Button * button = new gcn::Button("Wield");
  button->setEventId("wield");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  button = new gcn::Button("Give");
  button->setEventId("give");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);

  button = new gcn::Button("Drop");
  button->setEventId("drop");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  button = new gcn::Button("Eat");
  button->setEventId("eat");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();
}

Inventory::~Inventory()
{
}

void Inventory::actionPressed(std::string event)
{
  int selected = m_items->getSelected();

  Character * chr = System::instance()->getCharacter();
  if (chr == 0) { return; }
  Eris::Avatar * av = chr->getAvatar();
  if (av == 0) { return; }
  if ((selected < 0) ||
      ((unsigned int)selected >= av->getEntity()->numContained())) {
    return;
  }
  std::string name = av->getEntity()->getContained(selected)->getName();

  if (event == "wield") {
    std::string cmd("/wield ");
    cmd += name;
    System::instance()->runCommand(cmd);
  } else if (event == "give") {
    std::string cmd("/give 1 ");
    cmd += name;
    System::instance()->runCommand(cmd);
  } else if (event == "drop") {
    std::string cmd("/drop 1 ");
    cmd += name;
    System::instance()->runCommand(cmd);
  } else if (event == "eat") {
    std::string cmd("/eat ");
    cmd += name;
    System::instance()->runCommand(cmd);
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
