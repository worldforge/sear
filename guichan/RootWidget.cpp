// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/RootWidget.h"

#include <iostream>

namespace Sear {

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
    if ((y + child->getHeight()) > (old_height * 2 / 3)) {
      y += height - old_height;
    }
    child->setX(x);
    child->setY(y);
  }
}

} // namespace Sear
