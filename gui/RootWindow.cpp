// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/RootWindow.h"

namespace Sear {

RootWindow::RootWindow() {
}

RootWindow::~RootWindow() {
}

void RootWindow::render(Render * renderer)
{
    std::set<Window*>::const_iterator I = m_children.begin();
    for(; I != m_children.end(); ++I) {
        (*I)->render(renderer);
    }
}

} // namespace Sear
