// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_LOGIN_DIALOGUE_H
#define SEAR_GUI_LOGIN_DIALOGUE_H

#include "gui/Dialogue.h"

#include <sigc++/signal.h>

#include <string>

namespace Sear {

class Button;
class TextEntry;

/// Dialogue for connecting to the server
class LoginDialogue : public Dialogue {
protected:
  Button * m_loginButton;
  Button * m_createButton;
  TextEntry * m_usernameEntry;
  TextEntry * m_passwordEntry;
  TextEntry * m_realnameEntry;

  void login();
  void create();
public:
  LoginDialogue();
  virtual ~LoginDialogue();

  sigc::signal2<void, const std::string &, const std::string &> Login;
  sigc::signal3<void, const std::string &, const std::string &, const std::string &> Create;
};

} // namespace Sear

#endif // SEAR_GUI_LOGIN_DIALOGUE_H
