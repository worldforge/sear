// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Panel.h"

#include "gui/Box.h"
#include "gui/Button.h"

#include <iostream>

namespace Sear {

Panel::Panel() : Toplevel("Panel", 0), m_buttonBox(new VBox(0))
{
  setContents(m_buttonBox);
  setPos(1,1);
}

Panel::~Panel()
{
}

Button * Panel::addButton(Button * b)
{
  m_buttonBox->push_back(b);
  return b;
}

} // namespace Sear
