// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WORKSPACE_H
#define SEAR_GUI_WORKSPACE_H

#include "gui/Widget.h"

class RootWindow;

class Workspace : public Widget {
protected:
  RootWindow * m_rootWindow;
public:
  Workspace();
  virtual ~Workspace();
};

#endif // SEAR_GUI_WORKSPACE_H
