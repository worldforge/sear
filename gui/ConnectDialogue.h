// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_CONNECT_DIALOGUE_H
#define SEAR_GUI_CONNECT_DIALOGUE_H

#include "gui/Dialogue.h"

#include <sigc++/signal.h>

#include <string>

namespace Sear {

class Button;
class TextEntry;

/// Dialogue for connecting to the server
class ConnectDialogue : public Dialogue {
protected:
  Button * m_connectButton;
  TextEntry * m_hostEntry;

  void connect();
public:
  ConnectDialogue();
  virtual ~ConnectDialogue();

  sigc::signal1<void, const std::string &> Connect;
};

} // namespace Sear

#endif // SEAR_GUI_CONNECT_DIALOGUE_H
