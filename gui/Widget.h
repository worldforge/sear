// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WIDGET_H
#define SEAR_GUI_WIDGET_H

namespace Sear {

/// Higher level class defining any part in the gui.
/// They key difference is that some widgets may not have a visible component.
class Widget {
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

  virtual void show() = 0;
};

} // namespace Sear

#endif // SEAR_GUI_WIDGET_H
