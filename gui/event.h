// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_EVENT_H
#define SEAR_GUI_EVENT_H

namespace Sear {

static const unsigned int MOUSE_MOTION        = 1 <<  0;
static const unsigned int MOUSE_BUTTON_DOWN   = 1 <<  1;
static const unsigned int MOUSE_BUTTON_UP     = 1 <<  2;
static const unsigned int MOUSE_ENTER         = 1 <<  3;
static const unsigned int MOUSE_LEAVE         = 1 <<  4;
static const unsigned int KEY_PRESS           = 1 <<  5;

} // namespace Sear

#endif // SEAR_GUI_EVENT_H
