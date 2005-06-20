// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/LoginWindow.h"

#include "guichan/Alert.h"
#include "guichan/CharacterWindow.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"

#include "src/System.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

LoginWindow::LoginWindow() : gcn::Window("Login")
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  gcn::Box * vbox = new gcn::VBox(6);

  gcn::Box * hbox = new gcn::HBox(6);
  gcn::Label * l1 = new gcn::Label("UserName");
  m_userField = new gcn::TextField("                ");
  m_userField->setText("");
  hbox->pack(l1);
  hbox->pack(m_userField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  gcn::Label * l2 = new gcn::Label("Password");
  m_pswdField = new gcn::TextField("                ");
  m_pswdField->setText("");
  hbox->pack(l2);
  hbox->pack(m_pswdField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  gcn::Label * l3 = new gcn::Label("        ");
  m_pswdConfirmField = new gcn::TextField("                ");
  m_pswdConfirmField->setText("");
  m_pswdConfirmField->setEnabled(false);
  hbox->pack(l3);
  hbox->pack(m_pswdConfirmField);

  vbox->pack(hbox);

  m_createCheck = new gcn::CheckBox("New Account");
  vbox->pack(m_createCheck);

  hbox = new gcn::HBox(6);
  m_loginButton = new gcn::Button("Login");
  m_loginButton->setFocusable(false);
  m_loginListener = new ActionListenerSigC;
  SigC::Slot0<void> s1 = SigC::bind<action>(SigC::slot(*this, &LoginWindow::actionPressed), LOGIN);
  SigC::Slot1<void, std::string> s2 = SigC::hide<std::string>(s1);
  m_loginListener->Action.connect(s2);
  m_loginButton->addActionListener(m_loginListener);
  hbox->pack(m_loginButton);

  m_cancelButton = new gcn::Button("Cancel");
  m_cancelButton->setFocusable(false);
  m_cancelListener = new ActionListenerSigC;
  SigC::Slot0<void> s3 = SigC::bind<action>(SigC::slot(*this, &LoginWindow::actionPressed), CANCEL);
  SigC::Slot1<void, std::string> s4 = SigC::hide<std::string>(s3);
  m_cancelListener->Action.connect(s4);
  m_cancelButton->addActionListener(m_cancelListener);
  hbox->pack(m_cancelButton);

  vbox->pack(hbox);

  hbox = new gcn::HBox();
  hbox->pack(vbox, 6);

  vbox = new gcn::VBox();
  vbox->pack(hbox, 6);

  setContent(vbox);

  resizeToContent();
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::logic()
{
  m_pswdConfirmField->setEnabled(m_createCheck->isMarked());
  gcn::Window::logic();
}

void LoginWindow::actionPressed(action a)
{
  bool close = false;
  bool next = false;
  bool password_error = false;

  const std::string & username = m_userField->getText();
  const std::string & password = m_pswdField->getText();
  if (username.empty() || password.empty()) {
    return;
  }
  std::string cmd;
  switch (a) {
    case LOGIN:
      if (m_createCheck->isMarked()) {
        if (password != m_pswdConfirmField->getText()) {
          password_error = true;
          break;
        }
        cmd = "/create ";
        cmd += username;
        cmd += " ";
        cmd += password;
        cmd += " Sear_User";
      } else {
        cmd = "/login ";
        cmd += username;
        cmd += " ";
        cmd += password;
      }
      System::instance()->runCommand(cmd);
      close = true;
      next = true;
      break;
    case CANCEL:
      close = true;
      break;
    default:
      break;
  };

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

  if (password_error) {
    new Alert(parent, "Passwords do not match");
    return;
  }

  if (!close) { return; }

  parent->remove(this);

  if (!next) { return; }

  CharacterWindow * cw = new CharacterWindow;
  parent->add(cw, parent->getWidth() / 2 - cw->getWidth() / 2,
                  parent->getHeight() / 2 - cw->getHeight() / 2);

}

} // namespace Sear
