// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_TOPLEVEL_H
#define SEAR_GUI_TOPLEVEL_H

#include "gui/Container.h"

#include <string>

namespace Sear {

class Window;

/// Widget at the level below the Root.
class Toplevel : public Container {
private:
  // Private and unimplemented to prevent slicing
  Toplevel(const Toplevel &);
  const Toplevel & operator=(const Toplevel &);
protected:
  Widget * m_contents;
  Window * m_frame;
  std::string m_title;

  static const int m_border = 4;
public:
  explicit Toplevel(const std::string & title);
  virtual ~Toplevel();

  Widget * getContents() const {
    return m_contents;
  }

  void setContents(Widget *);
  void setPos(int x, int y);

  virtual void show();
  virtual void map(Window * win, int x, int y, int & w, int & h);
};

} // namespace Sear

#endif // SEAR_GUI_TOPLEVEL_H
