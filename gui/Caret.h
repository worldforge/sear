// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_CARET_H
#define SEAR_GUI_CARET_H

#include "gui/Window.h"

#include <string>

namespace Sear {

/// A rectangular window
class Caret : public Window {
private:
  // Private and unimplemented to prevent slicing
  Caret(const Caret &);
  const Caret & operator=(const Caret &);
public:
  explicit Caret(int);
  virtual ~Caret();

  virtual void render(Render *);
};

} // namespace Sear

#endif // SEAR_GUI_CARET_H
