// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/CharacterWindow.h"

#include "guichan/LoginWindow.h"
#include "guichan/ActionListenerSigC.h"

#include "guichan/box.hpp"

#include <guichan/widgets/label.hpp>

#include "src/System.h"
#include "src/client.h"

#include <Eris/Account.h>

#include <Atlas/Objects/Entity.h>

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

class CharacterListModel : public gcn::ListModel
{
public:
  CharacterListModel()
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return; }
    account->refreshCharacterInfo();
  }

  virtual int getNumberOfElements()
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return 0; }
    return account->getCharacters().size();
  }

  virtual std::string getElementAt(int i)
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return ""; }
    const Eris::CharacterMap & ci = account->getCharacters();
    Eris::CharacterMap::const_iterator I = ci.begin();
    Eris::CharacterMap::const_iterator Iend = ci.end();
    for (int j = 0; I != Iend; ++I, ++j) {
      if (i == j) {
        return I->second->getName();
      }
    }
    return "UNKNOWN";
  }
};


CharacterWindow::CharacterWindow() : gcn::Window("Character selection"),
                                     m_selected(-1)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  vbox->pack(new gcn::Label("Characters"));

  m_characterListModel = new CharacterListModel;

  m_characters = new gcn::ListBox(m_characterListModel);
  m_characters->setWidth(200);
  m_characters->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_characters,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  scroll_area->setWidth(200);
  scroll_area->setHeight(200);
  scroll_area->setBorderSize(1);
  vbox->pack(scroll_area);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &CharacterWindow::actionPressed));

  m_refreshButton = new gcn::Button("Refresh");
  m_refreshButton->setFocusable(false);
  m_refreshButton->setEventId("refresh");
  m_refreshButton->addActionListener(m_buttonListener);
  vbox->pack(m_refreshButton);

  gcn::Box * hbox = new gcn::HBox(6);

  hbox->pack(new gcn::Label("Name"));
  m_nameField = new gcn::TextField("                ");
  m_nameField->setText("");
  hbox->pack(m_nameField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);

  hbox->pack(new gcn::Label("Type"));
  m_typeField = new gcn::TextField("                ");
  m_typeField->setText("");
  hbox->pack(m_typeField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);

  m_takeButton = new gcn::Button("Take character");
  m_takeButton->setFocusable(false);
  m_takeButton->setEventId("take");
  m_takeButton->addActionListener(m_buttonListener);
  hbox->pack(m_takeButton);

  m_createButton = new gcn::Button("Create character");
  m_createButton->setFocusable(false);
  m_createButton->setEventId("create");
  m_createButton->addActionListener(m_buttonListener);
  hbox->pack(m_createButton);

  vbox->pack(hbox);

  m_closeButton = new gcn::Button("Close");
  m_closeButton->setFocusable(false);
  m_closeButton->setEventId("close");
  m_closeButton->addActionListener(m_buttonListener);

  vbox->pack(m_closeButton);

  setContent(vbox);

  resizeToContent();
}

CharacterWindow::~CharacterWindow()
{
}

void CharacterWindow::logic()
{
  int new_selected = m_characters->getSelected();
  if (new_selected != m_selected) {
    m_selected = new_selected;
    m_nameField->setText("");
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account != 0) {
      const Eris::CharacterMap & ci = account->getCharacters();
      if (m_selected >= 0 && (unsigned int)m_selected < ci.size()) {
        Eris::CharacterMap::const_iterator I = ci.begin();
        Eris::CharacterMap::const_iterator Iend = ci.end();
        for (int j = 0; I != Iend; ++I, ++j) {
          if (m_selected == j) {
            m_nameField->setText(I->second->getName());
            m_typeField->setText(I->second->getParents().front());
          }
        }
      }
    }
  }
  gcn::Window::logic();
}

void CharacterWindow::actionPressed(std::string event)
{
  bool close = false;

  if (event == "take") {
    std::cout << "Character " << m_nameField->getText() << std::endl << std::flush;
    std::string cmd("/take ");
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account != 0) {
      const Eris::CharacterMap & ci = account->getCharacters();
      if (m_selected >= 0 && (unsigned int)m_selected < ci.size()) {
        Eris::CharacterMap::const_iterator I = ci.begin();
        Eris::CharacterMap::const_iterator Iend = ci.end();
        for (int j = 0; I != Iend; ++I, ++j) {
          if (m_selected == j) {
            cmd += I->second->getId();
            System::instance()->runCommand(cmd);
            close = true;
          }
        }
      }
    }
  } else if (event == "create") {
    std::string cmd("/add ");
    cmd += m_nameField->getText();
    cmd += " ";
    cmd += m_typeField->getText();
    cmd += " ";
    cmd += "male"; // FIXME add a widget for this
    cmd += " A settler"; // FIXME Could add a widget for this too.
    System::instance()->runCommand(cmd);
    close = true;
  } else if (event == "close") {
    close = true;
  } else if (event == "refresh") {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account != 0) {
      account->refreshCharacterInfo();
    }
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }

  if (!close) { return; }

  gcn::BasicContainer * parent_widget = getParent();
  if (parent_widget == 0) {
    std::cout << "NO PARENT" << std::endl << std::flush;
    return;
  }
  gcn::Container * parent = dynamic_cast<gcn::Container *>(parent_widget);
  if (parent == 0) {
    std::cout << "WEIRD PARENT" << std::endl << std::flush;
    return;
  }
  parent->remove(this);
}

} // namespace Sear
