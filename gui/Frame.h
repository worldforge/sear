// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_FRAME_H
#define SEAR_GUI_FRAME_H

#include "gui/Window.h"

namespace Sear {

/// A rectangular window
class Frame : public Window {
private:
  // Private and unimplemented to prevent slicing
  Frame(const Frame &);
  const Frame & operator=(const Frame &);
protected:
  bool m_down;
public:
  Frame();
  virtual ~Frame();

  void down() {
    m_down = true;
  }

  void up() {
    m_down = false;
  }

  virtual void render(Render *);
};

} // namespace Sear

#endif // SEAR_GUI_FRAME_H
