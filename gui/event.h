// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_EVENT_H
#define SEAR_GUI_EVENT_H

namespace Sear {

typedef enum event_mask {
  MOUSE_MOTION        = 1 <<  0,
  MOUSE_BUTTON_DOWN   = 1 <<  1,
  MOUSE_BUTTON_UP     = 1 <<  2,
  KEY_PRESS           = 1 <<  3,
} EventMask;

} // namespace Sear

#endif // SEAR_GUI_EVENT_H
