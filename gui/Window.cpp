// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Window.h"

#include <iostream>

namespace Sear {

Window::Window() : m_parent(0), m_visible(true) {
}

Window::~Window() {
}

void Window::addChild(Window * w)
{
  m_children.insert(w);
  w->m_parent = this;
}

void Window::removeChild(Window * w)
{
  m_children.erase(w);
  w->m_parent = 0;
}

void Window::setPos(int x, int y)
{
  m_x = x;
  m_y = y;
}

void Window::setSize(int w, int h)
{
  m_w = w;
  m_h = h;
}

void Window::setEvents(unsigned int ev)
{
  m_eventMask = ev;
}

void Window::mouseMotion(short x, short y, short ox, short oy)
{
  std::set<Window *>::const_iterator I = m_children.begin();
  for(; I != m_children.end(); ++I) {
    Window & w = **I;
    short rx = x - w.m_x,
          ry = y - w.m_y,
          rox = ox - w.m_x,
          roy = oy - w.m_y;
    if (((rx >= 0) && (rx < w.m_w) && (ry >= 0) && (ry < w.m_h)) ||
        ((rox >= 0) && (rox < w.m_w) && (roy >= 0) && (roy < w.m_h))) {
      w.mouseMotion(rx, ry, rox, roy);
    }
  }
  if (x < 0 || y < 0 || x >= m_w || y >= m_h) {
    if (m_eventMask & MOUSE_LEAVE) {
      MouseLeave.emit();
    }
  } else if (ox < 0 || oy < 0 || ox >= m_w || oy >= m_h) {
    if (m_eventMask & MOUSE_ENTER) {
      MouseEnter.emit();
    }
  }
}

void Window::mouseDown(short x, short y)
{
  std::set<Window *>::const_iterator I = m_children.begin();
  for(; I != m_children.end(); ++I) {
    Window & w = **I;
    short rx = x - w.m_x,
          ry = y - w.m_y;
    if ((rx < 0) || (rx >= w.m_w) ||
        (ry < 0) || (ry >= w.m_h)) {
      continue;
    }
    w.mouseDown(rx, ry);
  }
  if (m_eventMask & MOUSE_BUTTON_DOWN) {
    MouseDown.emit();
  }
}

void Window::mouseUp(short x, short y)
{
  std::set<Window *>::const_iterator I = m_children.begin();
  for(; I != m_children.end(); ++I) {
    Window & w = **I;
    short rx = x - w.m_x,
          ry = y - w.m_y;
    if ((rx < 0) || (rx >= w.m_w) ||
        (ry < 0) || (ry >= w.m_h)) {
      continue;
    }
    w.mouseUp(rx, ry);
  }
  if (m_eventMask & MOUSE_BUTTON_UP) {
    MouseUp.emit();
  }
}

void Window::keyPress(SDLKey ks, Uint16 ch)
{
  if (m_eventMask & KEY_PRESS) {
    KeyPress.emit(ks, ch);
  }
}

} // namespace Sear
