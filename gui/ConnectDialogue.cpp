// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/ConnectDialogue.h"

#include "gui/Button.h"
#include "gui/TextEntry.h"
#include "gui/Box.h"

#include <sigc++/object_slot.h>

namespace Sear {

ConnectDialogue::ConnectDialogue() : Dialogue("Connect to Server")
{
  m_connectButton = new Button("Connect");
  m_hostEntry = new TextEntry(20, "localhost");

  m_connectButton->Clicked.connect(SigC::slot(*this, &ConnectDialogue::connect));

  VBox * vb = new VBox(2, 4);
  vb->push_back(m_hostEntry);
  vb->push_back(m_connectButton);

  HBox * hb = new HBox(2, 4);
  hb->push_back(vb);

  setPane(hb);
}

ConnectDialogue::~ConnectDialogue()
{
}

void ConnectDialogue::connect()
{
  Connect.emit(m_hostEntry->input());
}

} // namespace Sear
