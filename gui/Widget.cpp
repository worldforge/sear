// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "gui/Widget.h"

#include <cassert>

namespace Sear {

Widget::Widget() : m_parent(0) {
}

Widget::~Widget() {
}

void Widget::setParent(Widget * w)
{
    assert(w != 0);
    assert(m_parent == 0);

    m_parent = w;
}

} // namespace Sear
