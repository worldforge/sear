// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WINDOW_H
#define SEAR_GUI_WINDOW_H

#include <set>

namespace Sear {

class Render;

/// Low level class defining any area of the screen in which we might be
/// interested
class Window {
protected:
  Window * m_parent;
  std::set<Window *> m_children;
  short m_x, m_y;
  short m_w, m_h;

  Window();

private:
  // Private and unimplemented to prevent slicing
  Window(const Window &);
  const Window & operator=(const Window &);
public:
  virtual ~Window();
  
  void addChild(Window *);
  void setPos(int, int);
  void setSize(int, int);

  virtual void render(Render *) = 0;
};

} // namespace Sear

#endif // SEAR_GUI_WINDOW_H
