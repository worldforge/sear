// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/ToggleButton.h"

#include "gui/Frame.h"

#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

ToggleButton::ToggleButton(const std::string & text, bool pressed) :
                                                     Button(text)
{
  m_pressed = pressed;
}

ToggleButton::ToggleButton(const Graphic & graphic, bool pressed) :
                                                    Button(graphic)
{
  m_pressed = pressed;
}

ToggleButton::~ToggleButton()
{
}

void ToggleButton::connectSignals()
{
  m_frame->MouseDown.connect(sigc::mem_fun(*this, &ToggleButton::onPressed));
  m_frame->MouseUp.connect(sigc::mem_fun(*this, &ToggleButton::onRelease));
  // m_frame->MouseLeave.connect(sigc::mem_fun(*this, &ToggleButton::onLeave));
  m_frame->setEvents(MOUSE_BUTTON_DOWN | MOUSE_BUTTON_UP | MOUSE_LEAVE);
}

void ToggleButton::onPressed()
{
  if (m_pressed) {
    m_pressed = false;
    m_frame->up();
  } else {
    m_pressed = true;
    m_frame->down();
  }
}

void ToggleButton::onRelease()
{
  if (m_pressed) {
    Clicked.emit();
  } else {
    UnClicked.emit();
  }
}

void ToggleButton::onLeave()
{
  // FIXME This doesn't work right yet
  if (m_pressed) {
    m_pressed = false;
    m_frame->up();
  } else {
    m_pressed = true;
    m_frame->down();
  }
}

} // namespace Sear
