// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_CONTAINER_H
#define SEAR_GUI_CONTAINER_H

#include "gui/Widget.h"

#include <set>

namespace Sear {

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class Container : public Widget {
protected:
  std::set<Widget *> m_children;

  Container();

private:
  // Private and unimplemented to prevent slicing
  Container(const Container &);
  const Container & operator=(const Container &);
public:
  virtual ~Container();

  virtual void render() = 0;
};

} // namespace Sear

#endif // SEAR_GUI_CONTAINER_H
