// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_LOGIN_WINDOW_H
#define SEAR_GUICHAN_LOGIN_WINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace gcn {

class Label;
class Button;
class TextField;
class CheckBox;

} // namespace gcn

namespace Sear {

class PasswordField;

class ActionListenerSigC;

class LoginWindow : virtual public SigC::Object, public gcn::Window {
protected:
  gcn::Button * m_loginButton;
  gcn::Button * m_cancelButton;
  gcn::TextField * m_userField;
  PasswordField * m_pswdField;
  PasswordField * m_pswdConfirmField;
  gcn::TextField * m_nameField;
  gcn::Label * m_nameLabel;
  gcn::CheckBox * m_createCheck;

  ActionListenerSigC * m_buttonListener;

  virtual void logic();
public:
  LoginWindow();
  virtual ~LoginWindow();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_LOGIN_WINDOW_H
