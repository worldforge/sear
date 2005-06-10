// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_ROOTWIDGET_H
#define SEAR_GUICHAN_ROOTWIDGET_H

#include <guichan.hpp>

namespace Sear {

class RootWidget : public gcn::Container
{
  public:
    bool childHasMouse();
};

} // namespace Sear

#endif // SEAR_GUICHAN_ROOTWIDGET_H
