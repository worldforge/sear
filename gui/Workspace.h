// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WORKSPACE_H
#define SEAR_GUI_WORKSPACE_H

#include "gui/Container.h"

namespace Sear {

class RootWindow;
class Toplevel;
class System;

class Workspace : public Container {
protected:
  System * m_system;
  RootWindow * m_rootWindow;
public:
  Workspace(System *);
  virtual ~Workspace();

  RootWindow * getRootWindow() const {
    return m_rootWindow;
  }

  void draw();
  void show();
  void addToplevel(Toplevel *);
};

} // namespace Sear

#endif // SEAR_GUI_WORKSPACE_H
