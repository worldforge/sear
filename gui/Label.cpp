// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Label.h"
#include "gui/String.h"

#include <iostream>

namespace Sear {

Label::Label(const std::string & text) : m_text(text)
{
}

Label::~Label()
{
}

void Label::map(Window * win, int x, int y, int & w, int & h)
{
  String * s = new String(m_text);
  s->setPos(x, y);
  win->addChild(s);
  w = s->w();
  h = s->h();
  std::cout << "Label::map return " << w << "," << h << std::endl << std::flush;
}

} // namespace Sear
