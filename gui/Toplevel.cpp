// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Toplevel.h"

#include "gui/Frame.h"

namespace Sear {

Toplevel::Toplevel(const std::string & title) : m_contents(0),
                                                m_frame(new Frame())
                                                m_title(title)
{

}

void Toplevel::setContents(Widget * w)
{
  m_contents = w;
  addChild(w);
}

} // namespace Sear
