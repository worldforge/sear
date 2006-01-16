// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/RootWidget.h"
#include "guichan/CommandLine.h"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include <iostream>

namespace Sear {

RootWidget::RootWidget()
{
    setBaseColor(gcn::Color(63, 63, 63, 191));
    setOpaque(false);
}

bool RootWidget::childHasMouse()
{
    return (mWidgetWithMouse != 0);
}

void RootWidget::resize(int width, int height, int old_width, int old_height)
{
  setDimension(gcn::Rectangle(0, 0, width, height));
  gcn::Container::WidgetIterator I = mWidgets.begin();
  gcn::Container::WidgetIterator Iend = mWidgets.end();
  for (; I != Iend; ++I) {
    gcn::Widget * child = *I;
    int x = child->getX(),
        y = child->getY();
    if ((x + child->getWidth() / 2) > (old_width * 2 / 3)) {
      x += width - old_width;
    }
    if (x > (width - 20)) {
      x = width - 20;
    }
    if (x < 0) {
      x = 0;
    }
    if ((y + child->getHeight()) > (old_height * 2 / 3)) {
      y += height - old_height;
    }
    if (y > (height - 20)) {
      y = height - 20;
    }
    if (y < 0) {
      y = 0;
    }
    child->setX(x);
    child->setY(y);
  }
}

void RootWidget::openWindow(gcn::Window * win)
{
  int x, y;
  Render * render = RenderSystem::getInstance().getRenderer();
  int width = render->getWindowWidth(),
      height = render->getWindowHeight();

  CoordDict::const_iterator I = m_coords.find(win->getCaption());

  if (I != m_coords.end()) {
    x = std::max(std::min(I->second.first, width - win->getWidth()), 0);
    y = std::max(std::min(I->second.second, height - win->getHeight()), 0);
  } else {
    x = width / 2 - win->getWidth() / 2;
    y = height / 2 - win->getHeight() / 2;
  }

  add(win, x, y);
}

void RootWidget::closeWindow(gcn::Window * win)
{
  m_coords[win->getCaption()] = std::make_pair(win->getX(), win->getY());
  remove(win);
}

void RootWidget::logic()
{
  // FIXME We can remove this entire function, and just turn Opaque when we
  // want it drawn.
  gcn::Widget * focus = mFocusHandler->getFocused();
  if (focus != 0 && focus != this && dynamic_cast<CommandLine *>(focus) == 0) {
    setOpaque(true);
  } else {
    setOpaque(false);
  }
  gcn::Container::logic();
}

} // namespace Sear
