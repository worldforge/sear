// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_LOGIN_WINDOW_H
#define SEAR_GUICHAN_LOGIN_WINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace gcn {

class Button;
class TextField;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;

class LoginWindow : virtual public SigC::Object, public gcn::Window {
protected:
  gcn::Button * m_loginButton;
  gcn::Button * m_createButton;
  gcn::TextField * m_userField;
  gcn::TextField * m_pswdField;

  ActionListenerSigC * m_loginListener;
  ActionListenerSigC * m_createListener;
public:
  enum action { LOGIN, CREATE };

  LoginWindow();
  virtual ~LoginWindow();

  void actionPressed(action a);
};

} // namespace Sear

#endif // SEAR_GUICHAN_LOGIN_WINDOW_H
