// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_FOCUS_H
#define SEAR_GUI_FOCUS_H

#include <sigc++/signal.h>

namespace Sear {

class Window;

void grabFocus(Window *);
void dropFocus(Window *);
Window * queryFocus();
SigC::Signal1<void, Window *> & focusSignal();

} // namespace Sear

#endif // SEAR_GUI_FOCUS_H
