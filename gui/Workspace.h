// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WORKSPACE_H
#define SEAR_GUI_WORKSPACE_H

#include "gui/Window.h"

/// This defines the gui level background of the window the game is running
/// in, similar to the desktop in a conventional windowing system.
class Workspace : public Window {
public:
  Workspace();
  virtual ~Workspace();
};

#endif // SEAR_GUI_WORKSPACE_H
