// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/TextEntry.h"

#include "gui/Frame.h"
#include "gui/String.h"

#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

TextEntry::TextEntry(int size, const std::string & text) :
           m_frame(new Frame()), m_text(new String(text)),
           m_size(size), m_border(4)
{
  m_frame->down();
}

TextEntry::~TextEntry()
{
}

void TextEntry::show()
{
  // Map the required widget sizes
}

void TextEntry::map(Window * win, int x, int y, int & w, int & h)
{
  m_text->setPos(m_border, m_border);
  int cw = m_size * 10, ch = 16;
  m_frame->addChild(m_text);
  m_frame->setPos(x, y);
  m_frame->setSize(m_border * 2 + cw, m_border * 2 + ch);
  // m_frame->MouseDown.connect(SigC::slot(*this, &TextEntry::onPressed));
  // m_frame->MouseUp.connect(SigC::slot(*this, &TextEntry::onRelease));
  m_frame->setEvents(MOUSE_BUTTON_DOWN | KEY_PRESS);
  win->addChild(m_frame);
  w = m_frame->w();
  h = m_frame->h();
  std::cout << "TextEntry::map return " << w << "," << h << std::endl << std::flush;
}

} // namespace Sear
