// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2007 Simon Goodall

#include "guichan/LoginWindow.h"

#include "guichan/Alert.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"
#include "guichan/passwordfield.h"
#include "guichan/Workarea.h"

#include "src/System.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

LoginWindow::LoginWindow() : gcn::Window("Login to server")
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(sigc::mem_fun(this, &LoginWindow::actionPressed));

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  gcn::Label * l1 = new gcn::Label("UserName");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  m_userField = new gcn::TextField("                ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_userField));
  m_userField->setText("");
  hbox->pack(l1);
  hbox->pack(m_userField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  gcn::Label * l2 = new gcn::Label("Password");
  m_widgets.push_back(SPtr<gcn::Widget>(l2));
  m_pswdField = new PasswordField("                ", m_buttonListener, gcn::ActionEvent(this, "login"));
  m_widgets.push_back(SPtr<gcn::Widget>(m_pswdField));
  m_pswdField->setText("");
  hbox->pack(l2);
  hbox->pack(m_pswdField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  gcn::Label * l3 = new gcn::Label("        ");
  m_widgets.push_back(SPtr<gcn::Widget>(l3));
  m_pswdConfirmField = new PasswordField("                ", NULL, gcn::ActionEvent(this, ""));
  m_widgets.push_back(SPtr<gcn::Widget>(m_pswdConfirmField));
  m_pswdConfirmField->setText("");
  m_pswdConfirmField->setEnabled(false);
  m_pswdConfirmField->setVisible(false);
  hbox->pack(l3);
  hbox->pack(m_pswdConfirmField);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  m_nameLabel = new gcn::Label("Name    ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_nameLabel));
  m_nameLabel->setVisible(false);
  m_nameField = new gcn::TextField("                ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_nameField));
  m_nameField->setText("");
  m_nameField->setEnabled(false);
  m_nameField->setVisible(false);
  hbox->pack(m_nameLabel);
  hbox->pack(m_nameField);

  vbox->pack(hbox);

  m_createCheck = new gcn::CheckBox("New Account");
  m_widgets.push_back(SPtr<gcn::Widget>(m_createCheck));
  vbox->pack(m_createCheck);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  m_loginButton = new gcn::Button("Login");
  m_widgets.push_back(SPtr<gcn::Widget>(m_loginButton));
  m_loginButton->setFocusable(false);
  m_loginButton->setActionEventId("login");
  m_loginButton->addActionListener(m_buttonListener);
  hbox->pack(m_loginButton);

  m_cancelButton = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(m_cancelButton));
  m_cancelButton->setFocusable(false);
  m_cancelButton->setActionEventId("close");
  m_cancelButton->addActionListener(m_buttonListener);
  hbox->pack(m_cancelButton);

  vbox->pack(hbox);

  hbox = new gcn::HBox();
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));
  hbox->pack(vbox, 6);

  vbox = new gcn::VBox();
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));
  vbox->pack(hbox, 6);

  add(vbox);

  resizeToContent();
}

LoginWindow::~LoginWindow()
{
  delete m_buttonListener;
}

void LoginWindow::logic()
{
  bool show_create = m_createCheck->isSelected();
  m_pswdConfirmField->setEnabled(show_create);
  m_pswdConfirmField->setVisible(show_create);
  m_nameField->setEnabled(show_create);
  m_nameField->setVisible(show_create);
  m_nameLabel->setVisible(show_create);
  gcn::Window::logic();
}

void LoginWindow::actionPressed(std::string event)
{
  bool close = false;
//  bool password_error = false;

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

  const std::string & username = m_userField->getText();
  const std::string & password = m_pswdField->getText();
  std::string cmd;
  if (event == "login") {
    if (username.empty()) {
        new Alert(parent, "No username specified");
    } else if (password.empty()) {
        new Alert(parent, "No password specified");
    } else {
      if (m_createCheck->isSelected()) {
        if (m_nameField->getText().empty()) {
          new Alert(parent, "No name specified");
        } else if (password == m_pswdConfirmField->getText()) {
          cmd = "/create ";
          cmd += username;
          cmd += " ";
          cmd += password;
          cmd += " ";
          cmd += m_nameField->getText();
          System::instance()->runCommand(cmd);
          close = true;
        } else {
          new Alert(parent, "Passwords do not match");
        }
      } else {
        cmd = "/login ";
        cmd += username;
        cmd += " ";
        cmd += password;
        System::instance()->runCommand(cmd);
        close = true;
      }
    }
  } else if (event == "close") {
    close = true;
  } else {
  }

  if (!close) { return; }

  //parent->remove(this);
  System::instance()->getWorkarea()->removeLater(this);
}

} // namespace Sear
