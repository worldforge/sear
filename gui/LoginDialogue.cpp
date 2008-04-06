// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/LoginDialogue.h"

#include "gui/Button.h"
#include "gui/TextEntry.h"
#include "gui/Box.h"
#include "gui/Label.h"

#include <sigc++/object_slot.h>

namespace Sear {

LoginDialogue::LoginDialogue() : Dialogue("Connect to Server")
{
  m_loginButton = new Button("Login");
  m_createButton = new Button("Create");
  m_usernameEntry = new TextEntry(10, "");
  m_passwordEntry = new TextEntry(10, "", true);
  m_realnameEntry = new TextEntry(20, "");

  m_loginButton->Clicked.connect(sigc::mem_fun(*this, &LoginDialogue::login));
  m_createButton->Clicked.connect(sigc::mem_fun(*this, &LoginDialogue::create));

  HBox * hb = new HBox(2);

  VBox * vb = new VBox(2);
  vb->push_back(new Label("Username"));
  vb->push_back(new Label("Password"));

  hb->push_back(vb);

  vb = new VBox(2);
  vb->push_back(m_usernameEntry);
  vb->push_back(m_passwordEntry);

  hb->push_back(vb);

  vb = new VBox(2, 4);
  vb->push_back(hb);

  vb->push_back(new Label("Full name"));
  vb->push_back(m_realnameEntry);

  hb = new HBox(2);

  hb->push_back(m_loginButton);
  hb->push_back(m_createButton);

  vb->push_back(hb);

  hb = new HBox(2, 4);
  hb->push_back(vb);

  setPane(hb);
}

LoginDialogue::~LoginDialogue()
{
}

void LoginDialogue::login()
{
  Login.emit(m_usernameEntry->input(), m_passwordEntry->input());
}

void LoginDialogue::create()
{
  Create.emit(m_usernameEntry->input(), m_passwordEntry->input(), m_realnameEntry->input());
}

} // namespace Sear
