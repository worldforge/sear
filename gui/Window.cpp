// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Window.h"

#include <iostream>

namespace Sear {

Window::Window() : m_parent(0) {
}

Window::~Window() {
}

void Window::addChild(Window * w)
{
    m_children.insert(w);
    w->m_parent = this;
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

void Window::mouseMotion(short x, short y)
{
  if (m_parent != 0) {
    std::cout << "CHILD " << x << " " << y << std::endl << std::flush;
  }
  std::set<Window *>::const_iterator I = m_children.begin();
  for(; I != m_children.end(); ++I) {
    Window & w = **I;
    short rx = x - w.m_x,
          ry = y - w.m_y;
    if ((rx < 0) || (rx >= w.m_w) ||
        (ry < 0) || (ry >= w.m_h)) {
      continue;
    }
    w.mouseMotion(rx, ry);
  }
}


} // namespace Sear
