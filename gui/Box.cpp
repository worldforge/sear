// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Box.h"

#include <iostream>

namespace Sear {

Box::Box(int packing) : m_packing(packing)
{
}

Box::~Box()
{
}

Widget * Box::push_back(Widget * w)
{
    m_boxContents.push_back(w);
    Container::addChild(w);
    return w;
}

VBox::VBox(int packing) : Box(packing)
{
}

VBox::~VBox()
{
}

void VBox::map(Window * win, int x, int y, int & w, int & h)
{
  int width = 0, height = 0;

  std::list<Widget *>::const_iterator I = m_boxContents.begin();
  std::list<Widget *>::const_iterator Iend = m_boxContents.end();
  for (; I != Iend; ++I) {
    int cw, ch;
    (*I)->map(win, x + 0, y + height, cw, ch);
    std::cout << "VBox::map mapping at 0," << height << " " << cw << "," << ch << std::endl << std::flush;
    width = std::max(width, cw);
    height += ch;
    height += m_packing;
  }
  w = width;
  h = height - m_packing;
  std::cout << "VBox::map returning " << w << "," << h << std::endl << std::flush;
}

HBox::HBox(int packing) : Box(packing)
{
}

HBox::~HBox()
{
}

void HBox::map(Window * win, int x, int y, int & w, int & h)
{
  int width = 0, height = 0;

  std::list<Widget *>::const_iterator I = m_boxContents.begin();
  std::list<Widget *>::const_iterator Iend = m_boxContents.end();
  for (; I != Iend; ++I) {
    int cw, ch;
    (*I)->map(win, x + width, y + 0, cw, ch);
    std::cout << "HBox::map mapping at 0," << height << " " << cw << "," << ch << std::endl << std::flush;
    width += cw;
    width += m_packing;
    height = std::max(height, ch);
  }
  w = width - m_packing;
  h = height;
  std::cout << "HBox::map returning " << w << "," << h << std::endl << std::flush;
}

} // namespace Sear
