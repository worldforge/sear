// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Toplevel.h"

#include "gui/Frame.h"
#include "gui/Workspace.h"
#include "gui/RootWindow.h"

#include <iostream>

namespace Sear {

static const int min_width = 20;
static const int min_height = 20;

Toplevel::Toplevel(const std::string & title) : m_contents(0),
                                                m_frame(new Frame()),
                                                m_title(title)
{
  m_frame->setSize(min_width, min_height);
}

Toplevel::~Toplevel()
{
}

void Toplevel::setContents(Widget * w)
{
  m_contents = w;
  addChild(w);
}

void Toplevel::setPos(int x, int y)
{
  m_frame->setPos(x, y);
}

void Toplevel::show()
{
  Container::show();

  Workspace * ws = dynamic_cast<Workspace *>(m_parent);

  if (ws == 0) {
    std::cerr << "Toplevel parent is not a Workspace"
              << std::endl << std::flush;
    return;
  }

  RootWindow * rw = ws->getRootWindow();
  int width, height;
  map(rw, 0, 0, width, height);
}

void Toplevel::map(Window * win, int x, int y, int & w, int & h)
{
  int cw = 0, ch = 0;
  m_contents->map(m_frame, m_border, m_border, cw, ch);
  m_frame->setSize(m_border * 2 + cw, m_border * 2 + ch);
  win->addChild(m_frame);
  w = m_frame->w();
  h = m_frame->h();
  std::cout << "Toplevel::map returning " << w << "," << h << std::endl << std::flush;
}

} // namespace Sear
