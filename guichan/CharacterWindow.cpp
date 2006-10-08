// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/CharacterWindow.h"
#include "guichan/DblClkListBox.h"

#include "guichan/Alert.h"
#include "guichan/LoginWindow.h"
#include "guichan/ActionListenerSigC.h"

#include "guichan/box.hpp"
#include "guichan/Workarea.h"

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

static const bool debug = false;

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

class TypeListModel : public gcn::ListModel
{
public:
  virtual int getNumberOfElements()
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return 0; }
    return account->getCharacterTypes().size();
  }

  virtual std::string getElementAt(int i)
  {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account == 0) { return ""; }
    const std::vector<std::string> & types = account->getCharacterTypes();
    if (i < types.size()) {
      return types[i];
    } else {
      return "UNKNOWN";
    }
  }
};

CharacterWindow::CharacterWindow() : gcn::Window("Character selection"),
                                     m_charSelected(-1), m_typeSelected(-1)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  gcn::Label *l1 = new gcn::Label("Characters");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  vbox->pack(l1);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &CharacterWindow::actionPressed));

  m_characterListModel = new CharacterListModel;

  m_characters = new DblClkListBox(m_characterListModel, m_buttonListener, "take");
  m_widgets.push_back(SPtr<gcn::Widget>(m_characters));
  m_characters->setWidth(200);
  m_characters->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_characters,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scroll_area));
  scroll_area->setWidth(200);
  scroll_area->setHeight(200);
  scroll_area->setBorderSize(1);
  vbox->pack(scroll_area);

  m_refreshButton = new gcn::Button("Refresh");
  m_widgets.push_back(SPtr<gcn::Widget>(m_refreshButton));
  m_refreshButton->setFocusable(false);
  m_refreshButton->setEventId("refresh");
  m_refreshButton->addActionListener(m_buttonListener);
  vbox->pack(m_refreshButton);

  gcn::Box * vbox1 = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox1));

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  l1 = new gcn::Label("Name");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  hbox->pack(l1);
  m_nameField = new gcn::TextField("                ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_nameField));
  m_nameField->setText("");
  hbox->pack(m_nameField);

  vbox1->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  l1 = new gcn::Label("Type");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  hbox->pack(l1);

  m_typeField = new gcn::TextField("                ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_typeField));
  m_typeField->setText("");
  hbox->pack(m_typeField);

  vbox1->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  hbox->pack(vbox1);

  TypeListModel * type_list_model = new TypeListModel;
  
  m_types = new gcn::ListBox(type_list_model);
  m_widgets.push_back(SPtr<gcn::Widget>(m_types));
  scroll_area = new gcn::ScrollArea(m_types,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scroll_area));
  scroll_area->setWidth(100);
  scroll_area->setHeight(vbox1->getHeight());
  scroll_area->setBorderSize(1);
  hbox->pack(scroll_area);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  m_charButton = new gcn::Button("Create new character");
  m_widgets.push_back(SPtr<gcn::Widget>(m_charButton));
  m_charButton->setFocusable(false);
  m_charButton->setEventId("create");
  m_charButton->addActionListener(m_buttonListener);
  hbox->pack(m_charButton);

  vbox->pack(hbox);

  m_closeButton = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(m_closeButton));
  m_closeButton->setFocusable(false);
  m_closeButton->setEventId("close");
  m_closeButton->addActionListener(m_buttonListener);

  vbox->pack(m_closeButton);

  add(vbox);

  resizeToContent();
}

CharacterWindow::~CharacterWindow()
{
  delete m_characterListModel;
  delete m_types->getListModel();
  delete m_buttonListener;
}

void CharacterWindow::logic()
{
  int new_char_sel = m_characters->getSelected();
  if (new_char_sel != m_charSelected) {
    m_charSelected = new_char_sel;
    if (new_char_sel >= 0) {
      m_nameField->setText("");
      Eris::Account * account = System::instance()->getClient()->getAccount();
      if (account != 0) {
        const Eris::CharacterMap & ci = account->getCharacters();
        if (m_charSelected >= 0 && (unsigned int)m_charSelected < ci.size()) {
          Eris::CharacterMap::const_iterator I = ci.begin();
          Eris::CharacterMap::const_iterator Iend = ci.end();
          for (int j = 0; I != Iend; ++I, ++j) {
            if (m_charSelected == j) {
              m_nameField->setText(I->second->getName());
              m_typeField->setText(I->second->getParents().front());
              m_charButton->setCaption("Take character");
              m_charButton->setEventId("take");
            }
          }
        }
      }
      m_types->setSelected(-1);
    }
  }
  if (new_char_sel >= 0) {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account != 0) {
      const Eris::CharacterMap & ci = account->getCharacters();
      if (m_charSelected >= 0 && (unsigned int)m_charSelected < ci.size()) {
        Eris::CharacterMap::const_iterator I = ci.begin();
        Eris::CharacterMap::const_iterator Iend = ci.end();
        for (int j = 0; I != Iend; ++I, ++j) {
          if (m_charSelected == j) {
            if (m_nameField->getText() != I->second->getName() ||
                m_typeField->getText() != I->second->getParents().front()) {
              m_characters->setSelected(-1);
              m_charButton->setCaption("Create new character");
              m_charButton->setEventId("create");
            }
          }
        }
      }
    }
  }
  int new_type_sel = m_types->getSelected();
  if (new_type_sel != m_typeSelected) {
    m_typeSelected = new_type_sel;
    if (new_type_sel >= 0) {
      m_typeField->setText("");
      Eris::Account * account = System::instance()->getClient()->getAccount();
      if (account != 0) {
        const std::vector<std::string> & types = account->getCharacterTypes();
        if (m_typeSelected < types.size()) {
          m_typeField->setText(types[m_typeSelected]);
        }
      }
      m_characters->setSelected(-1);
      m_charButton->setCaption("Create new character");
      m_charButton->setEventId("create");
    }
  }
  if (new_type_sel >= 0) {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account != 0) {
      const std::vector<std::string> & types = account->getCharacterTypes();
      if (new_type_sel >= 0 && m_typeSelected < types.size()) {
        if (m_typeField->getText() != types[m_typeSelected]) {
          m_types->setSelected(-1);
        }
      }
    }
  }
  gcn::Window::logic();
}

void CharacterWindow::actionPressed(std::string event)
{
  bool close = false;

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

  if (event == "take") {
    if (debug) std::cout << "Character " << m_nameField->getText() << std::endl << std::flush;
    std::string cmd("/take ");
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account != 0) {
      const Eris::CharacterMap & ci = account->getCharacters();
      if (m_charSelected >= 0 && (unsigned int)m_charSelected < ci.size()) {
        Eris::CharacterMap::const_iterator I = ci.begin();
        Eris::CharacterMap::const_iterator Iend = ci.end();
        for (int j = 0; I != Iend; ++I, ++j) {
          if (m_charSelected == j) {
            cmd += I->second->getId();
            System::instance()->runCommand(cmd);
            close = true;
          }
        }
      } else {
        new Alert(parent, "Please select a character to take.");
      }
    } else {
      new Alert(parent, "Not currently logged in.");
    }
  } else if (event == "create") {
    if (m_nameField->getText().empty() || m_typeField->getText().empty()) {
      new Alert(parent, "Please specify a name and type for the character to be created.");
    } else {
      std::string cmd("/add ");
      cmd += m_nameField->getText();
      cmd += " ";
      cmd += m_typeField->getText();
      cmd += " ";
      cmd += "male"; // FIXME add a widget for this
      cmd += " A settler"; // FIXME Could add a widget for this too.
      System::instance()->runCommand(cmd);
      close = true;
    }
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

//  parent->remove(this);
  System::instance()->getWorkarea()->removeLater(this);
}

} // namespace Sear
