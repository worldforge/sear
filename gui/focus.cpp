// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "focus.h"

#include <cassert>

namespace Sear {

static Window * focusWindow = 0;
SigC::Signal1<void, Window *> focusChangedSignal;

void grabFocus(Window * w)
{
  assert(w != 0);
  focusWindow = w;
  focusChangedSignal.emit(focusWindow);
}

void dropFocus(Window * w)
{
  assert(w != 0);
  if (w == focusWindow) {
    focusWindow = 0;
    focusChangedSignal.emit(focusWindow);
  }
}

Window * queryFocus()
{
  return focusWindow;
}

SigC::Signal1<void, Window *> & focusSignal()
{
  return focusChangedSignal;
}

} // namespace Sear
