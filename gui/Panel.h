// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_PANEL_H
#define SEAR_GUI_PANEL_H

#include "gui/Toplevel.h"

#include <string>

namespace Sear {

class Button;
class VBox;

/// Toplevel widget as the main permanent panel.
class Panel : public Toplevel {
protected:
  VBox * m_buttonBox;
public:
  Panel();
  virtual ~Panel();

  Button * addButton(Button * b);
};

} // namespace Sear

#endif // SEAR_GUI_PANEL_H
