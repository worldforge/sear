// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WINDOW_H
#define SEAR_GUI_WINDOW_H

#include <set>

/// Low level class defining any area of the screen in which we might be
/// interested
class Window {
protected:
  Window * m_parent;
  std::set<Window *> m_children;

  Window();

private:
  // Private and unimplemented to prevent slicing
  Window(const Window &);
  const Window & operator=(const Window &);
public:
  virtual ~Window();
};

#endif // SEAR_GUI_WINDOW_H
