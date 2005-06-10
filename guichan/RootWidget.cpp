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

} // namespace Sear
