// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/LoginWindow.h"
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
  m_loginButton = new gcn::Button("Login");
  m_loginButton->setFocusable(false);
  m_loginListener = new ActionListenerSigC;
  SigC::Slot0<void> s1 = SigC::bind<action>(SigC::slot(*this, &LoginWindow::actionPressed), LOGIN);
  SigC::Slot1<void, std::string> s2 = SigC::hide<std::string>(s1);
  m_loginListener->Action.connect(s2);
  m_loginButton->addActionListener(m_loginListener);
  hbox->pack(m_loginButton);

  m_createButton = new gcn::Button("Create");
  m_createButton->setFocusable(false);
  m_createListener = new ActionListenerSigC;
  SigC::Slot0<void> s3 = SigC::bind<action>(SigC::slot(*this, &LoginWindow::actionPressed), CREATE);
  SigC::Slot1<void, std::string> s4 = SigC::hide<std::string>(s3);
  m_createListener->Action.connect(s4);
  m_createButton->addActionListener(m_createListener);
  hbox->pack(m_createButton);

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

void LoginWindow::actionPressed(action a)
{
  const std::string & username = m_userField->getText();
  const std::string & password = m_pswdField->getText();
  if (username.empty() || password.empty()) {
    return;
  }
  std::string cmd;
  switch (a) {
    case LOGIN:
      std::cout << "LOGIN " << username << " " << password << std::endl << std::flush;
      cmd = "/login ";
      cmd += username;
      cmd += " ";
      cmd += password;
      System::instance()->runCommand(cmd);
      break;
    case CREATE:
      std::cout << "CREATE " << username << " " << password << std::endl << std::flush;
      cmd = "/create ";
      cmd += username;
      cmd += " ";
      cmd += password;
      cmd += " Sear Player";
      // FIXME Actually this really needs another dialogue with password
      // confirmation.
      System::instance()->runCommand(cmd);
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
  parent->remove(this);
}

} // namespace Sear
