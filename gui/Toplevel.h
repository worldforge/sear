// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_TOPLEVEL_H
#define SEAR_GUI_TOPLEVEL_H

#include "gui/Container.h"

namespace Sear {

/// Widget at the level below the Root.
class Toplevel : public Container {
private:
  // Private and unimplemented to prevent slicing
  Toplevel(const Toplevel &);
  const Toplevel & operator=(const Toplevel &);
protected:
  Widget * m_contents;
  Window * m_frame;
  Window * m_title;

public:
  explicit Toplevel(const std::string & title);
  virtual ~Toplevel();

  Window * getContents() const {
    return m_contents;
  }

  void setContents(Widget *);
};

} // namespace Sear

#endif // SEAR_GUI_TOPLEVEL_H
