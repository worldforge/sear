// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WIDGET_H
#define SEAR_GUI_WIDGET_H

#include <sigc++/object.h>

namespace Sear {

class Window;

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class Widget : virtual public sigc::trackable {
private:
  // Private and unimplemented to prevent slicing
  Widget(const Widget &);
  const Widget & operator=(const Widget &);
protected:
  Widget * m_parent;

  Widget();

public:
  virtual ~Widget();

  void setParent(Widget * );

  /// Create necessary window primitives, and map them into the space provided.
  ///
  /// @param w window this widget should be mapped into
  /// @param x x coordinate of area to be used in parent window
  /// @param y y coordinate of area to be used in parent window
  /// @param w reference used to store with of widget once mapped
  /// @param h reference used to store with of widget once mapped
  virtual void map(Window * win, int x, int y, int & w, int & h) = 0;
};

} // namespace Sear

#endif // SEAR_GUI_WIDGET_H
