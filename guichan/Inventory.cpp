// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2009 Simon Goodall

#include "common/Utility.h"

#include "guichan/Inventory.h"
#include "guichan/RenameDialog.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/ActionImageBox.h"
#include "guichan/box.hpp"
#include "guichan/adjustingcontainer.hpp"

#include "src/System.h"
#include "src/Character.h"

#include <Eris/Avatar.h>

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include "renderers/RenderSystem.h"
#include "renderers/TextureManager.h"

#include <iostream>

static const gcn::Color defaultColour = gcn::Color(0,0,0);
static const gcn::Color highlightColour = gcn::Color(0,0,255);

namespace Sear {

Inventory::Inventory() : gcn::Window("Inventory")
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(sigc::mem_fun(*this, &Inventory::buttonPressed));

  m_imageListener = new ActionListenerSigC;
  m_imageListener->Action.connect(sigc::mem_fun(*this, &Inventory::actionPressed));

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  m_grid = new gcn::contrib::AdjustingContainer();
  m_grid->setNumberOfColumns(8);
  m_grid->setVerticalSpacing(4);
  m_grid->setHorizontalSpacing(4);
  m_grid->setPadding(2,2,2,2);

  m_grid->setWidth(512);
  m_grid->setHeight(128);
// 8 * imagebox + 9 padding (8+1)
  m_grid->setWidth(64 * 8 + 9);
  m_grid->setHeight(64 * 16);
  gcn::ScrollArea * scroll_area2 = new gcn::ScrollArea(m_grid,
                                      gcn::ScrollArea::SHOW_ALWAYS,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scroll_area2));
  scroll_area2->setWidth(512);
  scroll_area2->setHeight(128);
  scroll_area2->setFrameSize(1);
  scroll_area2->setFocusable(false);
  vbox->pack(scroll_area2);


  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));


  gcn::Button * button = new gcn::Button("Wield");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setActionEventId("wield");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  button = new gcn::Button("Give");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setActionEventId("give");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  button = new gcn::Button("Drop");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setActionEventId("drop");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  button = new gcn::Button("Eat");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setActionEventId("eat");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);


  button = new gcn::Button("Combine");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setActionEventId("combine");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  button = new gcn::Button("Divide");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setActionEventId("divide");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);



  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  button = new gcn::Button("Rename");
  m_widgets.push_back(SPtr<gcn::Widget>(button));
  button->setActionEventId("rename");
  button->setFocusable(false);
  button->addActionListener(m_buttonListener);
  hbox->pack(button);

  vbox->pack(hbox);
  add(vbox);

  resizeToContent();
}

Inventory::~Inventory()
{
  delete m_imageListener;
  delete m_buttonListener;
}

void Inventory::actionPressed(std::string event)
{
  m_selected = event;
}

void Inventory::buttonPressed(std::string event)
{
  std::string name = m_selected;

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
  } else if (event == "rename") {
    RenameDialog *rd = new RenameDialog(name, name);
    gcn::Container *parent =  dynamic_cast<gcn::Container *>(this->getParent());
    parent->add(rd, parent->getWidth() / 2 - getWidth() / 2,
                      parent->getHeight() / 2 - getHeight() / 2);
    
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

void Inventory::logic() {

  Character * chr = System::instance()->getCharacter();
  if (chr == 0) { 
    gcn::Window::logic();
    return;
  }
  const Character::InventoryMap &imap = chr->getInventoryMap();

  Character::InventoryMap::const_iterator I = imap.begin();
  Character::InventoryMap::const_iterator Iend = imap.end();

  size_t count = 0;
  size_t visible = 0;
  while (I != Iend) {
    if (count == m_images.size()) {
      ActionImageBox *b = new ActionImageBox("inv_missing");
      m_grid->add(b);//, 1);
      b->setWidth(64);
      b->setHeight(64);
// Note this makes the image too small. Needs to get widh
      b->setFrameSize(2);
      m_images.push_back(SPtr<ActionImageBox>(b));
      b->addActionListener(m_imageListener);
      b->setBaseColor(defaultColour);
    }
    if (I->first == m_selected) {
      m_images[count]->setBaseColor(highlightColour);
    } else {
      m_images[count]->setBaseColor(defaultColour);
    }
    m_images[count]->setVisible(true);
    m_images[count]->setActionEventId(I->first);
    m_images[count]->setText(string_fmt(I->second));

    std::string name = "inv_" + I->first;
    if (RenderSystem::getInstance().getTextureManager()->isTextureName(name)) {
      m_images[count]->setTextureName(name);
    } else {
      m_images[count]->setTextureName("inv_missing");
    }
    ++count;
    ++I;
    ++visible;
  }
  for (; count < m_images.size(); ++count) {
    m_images[count]->setVisible(false);
  }
  m_grid->setHeight(64 * (1+(visible /8)));

  gcn::Window::logic();
}

} // namespace Sear
