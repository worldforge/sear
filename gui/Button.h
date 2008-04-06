// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_BUTTON_H
#define SEAR_GUI_BUTTON_H

#include "gui/Container.h"

#include <sigc++/signal.h>

namespace Sear {

class Graphic;
class Frame;

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class Button : public Container {
private:
  // Private and unimplemented to prevent slicing
  Button(const Button &);
  const Button & operator=(const Button &);
protected:
  Frame * m_frame;
  unsigned int m_border;
  Widget * m_contents; // What are the contents?
  bool m_pressed;

public:
  explicit Button(const std::string & text);
  explicit Button(const Graphic & graphic);
  virtual ~Button();

  virtual void map(Window * win, int x, int y, int & w, int & h);

  virtual void connectSignals();

  // Called by event callbacks
  void onPressed();
  void onRelease();
  void onLeave();

  sigc::signal0<void> Clicked;
};

} // namespace Sear

#endif // SEAR_GUI_BUTTON_H
