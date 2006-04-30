// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/Utility.h"

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
    const Character::InventoryMap &imap = chr->getInventoryMap();
    return imap.size();
//    Eris::Avatar * av = chr->getAvatar();
//    if (av == 0) { return 0; }
//    return av->getEntity()->numContained();
  }
  virtual std::string getElementAt(int i)
  {
    Character * chr = System::instance()->getCharacter();
    if (chr == 0) { return ""; }
    const Character::InventoryMap &imap = chr->getInventoryMap();
    if ((unsigned int)i >= imap.size()) return  "";
    Character::InventoryMap::const_iterator  I = imap.begin();
    for (int n = 0; n < i; ++n, ++I);
    return I->first + " (" + string_fmt(I->second) + ")";
  }

  std::string getElementNameAt(int i)
  {
    Character * chr = System::instance()->getCharacter();
    if (chr == 0) { return ""; }
    const Character::InventoryMap &imap = chr->getInventoryMap();
    if (i >= imap.size()) return  "";
    Character::InventoryMap::const_iterator  I = imap.begin();
    for (int n = 0; n < i; ++n, ++I);
    return I->first;
  }
};


Inventory::Inventory() : gcn::Window("Inventory")
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  m_inventory = new InventoryListAdaptor;
  m_items = new gcn::ListBox(m_inventory);
  m_widgets.push_back(SPtr<gcn::Widget>(m_items));
  m_items->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_items,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scroll_area));
  scroll_area->setWidth(120);
  scroll_area->setHeight(100);
  scroll_area->setBorderSize(1);
  scroll_area->setFocusable(false);
  vbox->pack(scroll_area);

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &Inventory::actionPressed));

  gcn::Button * button = new gcn::Button("Wield");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setEventId("wield");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  button = new gcn::Button("Give");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setEventId("give");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  button = new gcn::Button("Drop");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setEventId("drop");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  button = new gcn::Button("Eat");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
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
  delete m_inventory;
  delete m_buttonListener;
}

void Inventory::actionPressed(std::string event)
{
  int selected = m_items->getSelected();

  if ((selected < 0) ||
        (selected >= m_inventory->getNumberOfElements())) {
    return;
  }
  std::string name = m_inventory->getElementNameAt(selected);

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
