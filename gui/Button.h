// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_BUTTON_H
#define SEAR_GUI_BUTTON_H

#include "gui/Container.h"

namespace Sear {

class Graphic;

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class Button : public Container {
private:
  // Private and unimplemented to prevent slicing
  Button(const Button &);
  const Button & operator=(const Button &);
public:
  explicit Button(const std::string & text);
  explicit Button(const Graphic & graphic);
  virtual ~Button();

  virtual void show();
};

} // namespace Sear

#endif // SEAR_GUI_WIDGET_H
