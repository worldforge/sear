// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Container.h"

namespace Sear {

Container::Container()
{
}

Container::~Container()
{
}

void Container::addChild(Widget * w)
{
  w->setParent(this);
  m_children.insert(w);
}

} // namespace Sear
