// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Dialogue.h"

#include "gui/Box.h"
#include "gui/Button.h"

#include <iostream>
#include <cassert>

namespace Sear {

Dialogue::Dialogue(const std::string & title) : Toplevel(title, 0),
                                                m_contentBox(new VBox(0))
{
  setContents(m_contentBox);

  HBox * hb = new HBox(0);
  m_contentBox->push_back(hb);

  hb->push_back(new Button("O"));
  hb->push_back(new Button(title));
  hb->push_back(new Button("C"));
}

Dialogue::~Dialogue()
{
}

void Dialogue::setPane(Widget * w)
{
  assert(m_contentBox->size() == 1);
  m_contentBox->push_back(w);
}

} // namespace Sear
