// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_TOGGLE_BUTTON_H
#define SEAR_GUI_TOGGLE_BUTTON_H

#include "gui/Button.h"

namespace Sear {

/// Button that's state is toggled by clicking
class ToggleButton : public Button {
private:
  // Private and unimplemented to prevent slicing
  ToggleButton(const ToggleButton &);
  const ToggleButton & operator=(const ToggleButton &);
public:
  explicit ToggleButton(const std::string & text, bool pressed = false);
  explicit ToggleButton(const Graphic & graphic, bool pressed = false);
  virtual ~ToggleButton();

  virtual void connectSignals();

  // Called by event callbacks
  void onPressed();
  void onRelease();
  void onLeave();

  sigc::signal0<void> UnClicked;
};

} // namespace Sear

#endif // SEAR_GUI_TOGGLE_BUTTON_H
