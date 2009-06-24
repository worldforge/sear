// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2006 - 2007 Simon Goodall

#include "guichan/CharacterWindow.h"
#include "guichan/CharacterListModel.h"
#include "guichan/TypeListModel.h"
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

CharacterWindow::CharacterWindow() : gcn::Window("Character selection"),
                                     m_charSelected(-1), m_typeSelected(-1)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

//  gcn::Box * vbox = new gcn::VBox(6);
//  m_widgets.push_back(SPtr<gcn::Widget>(vbox));
  int y_pos = 0;
  gcn::Label *l1 = new gcn::Label("Characters");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  l1->setPosition(0,y_pos);
  add(l1);
  y_pos += l1->getHeight() + 4;
//  vbox->pack(l1);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(sigc::mem_fun(*this, &CharacterWindow::actionPressed));

  m_characterListModel = new CharacterListModel;

  m_characters = new DblClkListBox(m_characterListModel, m_buttonListener, gcn::ActionEvent(this, "take"));
  m_widgets.push_back(SPtr<gcn::Widget>(m_characters));
  m_characters->setWidth(200);
  m_characters->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_characters,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scroll_area));
  scroll_area->setWidth(200);
  scroll_area->setHeight(200);
  scroll_area->setFrameSize(1);
  scroll_area->setPosition(0,y_pos);
  add(scroll_area);
  y_pos += scroll_area->getHeight() + 4;

  m_refreshButton = new gcn::Button("Refresh");
  m_widgets.push_back(SPtr<gcn::Widget>(m_refreshButton));
  m_refreshButton->setFocusable(false);
  m_refreshButton->setActionEventId("refresh");
  m_refreshButton->addActionListener(m_buttonListener);
  m_refreshButton->setPosition(0,y_pos);
  add(m_refreshButton);
  y_pos += m_refreshButton->getHeight() + 4;

  l1 = new gcn::Label("Name");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  l1->setPosition(0, y_pos);
  add(l1);

  m_nameField = new gcn::TextField("                ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_nameField));
  m_nameField->setText("");
  m_nameField->setPosition(l1->getWidth() + 2,y_pos);
  add(m_nameField);
  y_pos += m_nameField->getHeight() + 4;

  l1 = new gcn::Label("Type");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  l1->setPosition(0, y_pos);
  add(l1);

  TypeListModel * type_list_model = new TypeListModel;
  m_types = new gcn::DropDown(type_list_model);
  m_widgets.push_back(SPtr<gcn::Widget>(m_types));
 
  m_types->setPosition(l1->getWidth() + 4, y_pos);
  add(m_types);
  y_pos += m_types->getHeight() + 4;

  m_charButton = new gcn::Button("Create new character");
  m_widgets.push_back(SPtr<gcn::Widget>(m_charButton));
  m_charButton->setFocusable(false);
  m_charButton->setActionEventId("create");
  m_charButton->addActionListener(m_buttonListener);
  m_charButton->setPosition(0, y_pos);
  add(m_charButton);
  y_pos += m_charButton->getHeight() + 4;

  m_closeButton = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(m_closeButton));
  m_closeButton->setFocusable(false);
  m_closeButton->setActionEventId("close");
  m_closeButton->addActionListener(m_buttonListener);
  m_closeButton->setPosition(0, y_pos);
  add(m_closeButton);

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
      m_types->setSelected(-1);
      m_nameField->setText("");
      Eris::Account * account = System::instance()->getClient()->getAccount();
      if (account != 0) {
        const Eris::CharacterMap & ci = account->getCharacters();
        if (m_charSelected >= 0 && (unsigned int)m_charSelected < ci.size()) {
          Eris::CharacterMap::const_iterator I = ci.begin();
          Eris::CharacterMap::const_iterator Iend = ci.end();
          for (int j = 0; I != Iend; ++I, ++j) {
            if (m_charSelected == j) {
              const std::string &type_name = I->second->getParents().front();
              m_nameField->setText(I->second->getName());
              m_typeField = type_name;
              m_charButton->setCaption("Take character");
              m_charButton->setActionEventId("take");
              // Updates the type list box
              gcn::ListModel *lm =  m_types->getListModel();
              for (int i = 0; i < lm->getNumberOfElements(); ++i) {
                if (lm->getElementAt(i) == type_name) {
                  m_typeSelected = i;
                  m_types->setSelected(i);
                  break;
                }
              }
            }
          }
        }
      }
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
                m_typeField != I->second->getParents().front()) {
              m_characters->setSelected(-1);
              m_charButton->setCaption("Create new character");
              m_charButton->setActionEventId("create");
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
      m_typeField = "";
      Eris::Account * account = System::instance()->getClient()->getAccount();
      if (account != 0) {
        const std::vector<std::string> & types = account->getCharacterTypes();
        if ((size_t) m_typeSelected < types.size()) {
          m_typeField = types[m_typeSelected];
        }
      }
      m_characters->setSelected(-1);
      m_charButton->setCaption("Create new character");
      m_charButton->setActionEventId("create");
    }
  }
  if (new_type_sel >= 0) {
    Eris::Account * account = System::instance()->getClient()->getAccount();
    if (account != 0) {
      const std::vector<std::string> & types = account->getCharacterTypes();
      if (new_type_sel >= 0 && (size_t) m_typeSelected < types.size()) {
        if (m_typeField != types[m_typeSelected]) {
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

  gcn::Widget * parent_widget = getParent();
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
    if (m_nameField->getText().empty() || m_typeField.empty()) {
      new Alert(parent, "Please specify a name and type for the character to be created.");
    } else {
      std::string cmd("/add ");
      cmd += m_nameField->getText();
      cmd += " ";
      cmd += m_typeField;
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
