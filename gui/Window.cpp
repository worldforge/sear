// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Window.h"

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

} // namespace Sear
