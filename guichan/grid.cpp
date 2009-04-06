// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#include "grid.hpp"
#include "guichan.hpp"

namespace gcn {

Grid::~Grid()
{
}

void Grid::pack(Widget* child, int padding)
{
mSpacing = 2;
  int x = 0, y = 0;
//  WidgetPadding::const_iterator I = mWidgetPadding.begin();
//  WidgetPadding::const_iterator Iend = mWidgetPadding.end();
  std::list<gcn::Widget*>::const_iterator J = mWidgets.begin();
  std::list<gcn::Widget*>::const_iterator Jend = mWidgets.end();


x = mSpacing;
y = mSpacing;
  int count = 0;
  //for (; I != Iend && J != Jend; ++I, ++J) {
  for (; J != Jend;  ++J) {
  //  x = std::max(x, (*J)->getWidth());
//    x += *I * 2;
    x += (*J)->getWidth();
    x += mSpacing;
    count++;
    if (count > m_width) {
      count = 0;
      x = mSpacing;
      y += (*J)->getHeight();
      y += mSpacing;
 //     y += padding;
    }
  }
//  x += padding;

  add(child, x, y);
  mWidgetPadding.push_back(padding);

//  x = std::max(x, getWidth());
 // y = std::max(y, getHeight());
  y += child->getHeight();
  y += padding;

  x = std::max(x, getWidth());
  y = std::max(y, getHeight());
//  setSize(x, y);
}

} // end gcn
