// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_GRAPHIC_H
#define SEAR_GUI_GRAPHIC_H

#include "Widget.h"

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class Graphic : public Widget {
private:
  // Private and unimplemented to prevent slicing
  Graphic(const Graphic &);
  const Graphic & operator=(const Graphic &);
public:
  Graphic();
  virtual ~Graphic();

  virtual void render() = 0;
};

#endif // SEAR_GUI_GRAPHIC_H
