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
  Workspace * ws = dynamic_cast<Workspace *>(m_parent);

  if (ws == 0) {
    std::cerr << "Toplevel parent is not a Workspace"
              << std::endl << std::flush;
    return;
  }

  RootWindow * rw = ws->getRootWindow();
  rw->addChild(m_frame);
}

} // namespace Sear
