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

void Container::show()
{
  std::set<Widget *>::const_iterator I = m_children.begin();
  for(; I != m_children.end(); ++I) {
    (*I)->show();
  }
}

} // namespace Sear
