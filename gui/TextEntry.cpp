// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/TextEntry.h"

#include "gui/Frame.h"
#include "gui/Text.h"
#include "gui/Caret.h"
#include "gui/focus.h"

#include <sigc++/object_slot.h>

#include <iostream>
#include <algorithm>

namespace Sear {

TextEntry::TextEntry(int size, const std::string & text) :
           m_frame(new Frame()), m_text(new Text("")),
           m_caret(new Caret(16 * m_text->scale())), m_input(text),
           m_textOffset(0), m_size(size), m_border(2)
{
  m_frame->down();
  m_caretPos = m_input.size();
  setText();
}

TextEntry::~TextEntry()
{
}

void TextEntry::setText()
{
  if (m_caretPos > (m_size + m_textOffset)) {
    m_textOffset = m_caretPos - m_size;
  }
  if (m_caretPos < m_textOffset) {
    m_textOffset = m_caretPos ;
  }
  m_text->content() = m_input.substr(m_textOffset, std::min(m_size, m_input.size() - m_textOffset));
  m_caret->setPos((m_caretPos - m_textOffset) * 10 * m_text->scale() + m_border + 4, m_border);
}

void TextEntry::map(Window * win, int x, int y, int & w, int & h)
{
  m_text->setPos(m_border, m_border);
  int cw = m_size * 10 * m_text->scale(), ch = 16 * m_text->scale();
  m_frame->addChild(m_text);
  m_frame->setPos(x, y);
  m_frame->setSize(m_border * 2 + cw, m_border * 2 + ch);
  m_frame->MouseDown.connect(SigC::slot(*this, &TextEntry::onPressed));
  m_frame->KeyPress.connect(SigC::slot(*this, &TextEntry::onKeyPress));
  focusSignal().connect(SigC::slot(*this, &TextEntry::onFocus));
  m_frame->setEvents(MOUSE_BUTTON_DOWN | KEY_PRESS);
  m_caret->setPos(m_caretPos * 10 * m_text->scale() + m_border + 4, m_border);
  win->addChild(m_frame);
  w = m_frame->w();
  h = m_frame->h();
  std::cout << "TextEntry::map return " << w << "," << h << std::endl << std::flush;
}

void TextEntry::onKeyPress(SDLKey ks, Uint16 ch)
{
  if (ks == SDLK_BACKSPACE) {
    if(m_caretPos > 0) {
      m_text->content() = m_input.erase(m_caretPos - 1, 1);
      --m_caretPos;
    }
  } else if (ks == SDLK_LEFT) {
    if(m_caretPos > 0) {
      --m_caretPos;
    }
  } else if (ks == SDLK_RIGHT) {
    if(m_caretPos < m_input.size()) {
      ++m_caretPos;
    }
  } else if (ks == SDLK_RETURN || ks == SDLK_ESCAPE) {
    // nothing
  } else if (ch > 0 && ch < 0x80) {
    std::cout << "Key press" << std::endl << std::flush;
    m_input.insert(m_caretPos, 1, ch);
    ++m_caretPos;
  }
  setText();
}

void TextEntry::onPressed()
{
  std::cout << "TEXTENTRY PRESSED" << std::endl << std::flush;
  grabFocus(m_frame);
}

void TextEntry::onFocus(Window * w)
{
  std::cout << "FOCUS CHANGE" << std::endl << std::flush;
  if (w == m_frame) {
    m_frame->addChild(m_caret);
  } else {
    m_frame->removeChild(m_caret);
  }
}

} // namespace Sear
