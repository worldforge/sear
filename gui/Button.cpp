// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Button.h"

#include "gui/Label.h"
#include "gui/Frame.h"

#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

Button::Button(const std::string & text) : m_frame(new Frame()), m_border(2),
                                           m_pressed(false)
{
    m_contents = new Label(text);
    addChild(m_contents);
}

Button::Button(const Graphic & graphic)
{
  m_contents = 0;
}

Button::~Button()
{
}

void Button::map(Window * win, int x, int y, int & w, int & h)
{
  int cw = 0, ch = 0;
  m_contents->map(m_frame, m_border, m_border, cw, ch);
  m_frame->setPos(x, y);
  m_frame->setSize(m_border * 2 + cw, m_border * 2 + ch);
  m_frame->MouseDown.connect(SigC::slot(*this, &Button::onPressed));
  m_frame->MouseUp.connect(SigC::slot(*this, &Button::onRelease));
  m_frame->MouseLeave.connect(SigC::slot(*this, &Button::onLeave));
  m_frame->setEvents(MOUSE_BUTTON_DOWN | MOUSE_BUTTON_UP | MOUSE_LEAVE);
  win->addChild(m_frame);
  w = m_frame->w();
  h = m_frame->h();
  std::cout << "Button::map return " << w << "," << h << std::endl << std::flush;
}

void Button::onPressed()
{
  m_pressed = true;
  m_frame->down();
  std::cout << "Pressed" << std::endl << std::flush;
}

void Button::onRelease()
{
  std::cout << "Released" << std::endl << std::flush;
  if (m_pressed) {
    std::cout << "Clicked" << std::endl << std::flush;
    Clicked.emit();
  }
  m_frame->up();
  m_pressed = false;
}

void Button::onLeave()
{
  m_frame->up();
  m_pressed = false;
}

} // namespace Sear
