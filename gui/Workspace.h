// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WORKSPACE_H
#define SEAR_GUI_WORKSPACE_H

#include "gui/Container.h"

#include <SDL/SDL.h>

namespace Sear {

class Window;
class RootWindow;
class Toplevel;
class System;

typedef enum focus_policy {
  FOCUS_CLICK,
  FOCUS_SLOPPY,
} FocusPolicy;

class Workspace : public Container {
protected:
  System * m_system;
  RootWindow * m_rootWindow;
  FocusPolicy m_focusPolicy;
  short m_oldx, m_oldy;

  void mouseMotion(Window &, short, short, short, short);
  void mouseDown(Window &, short, short);
  void mouseUp(Window &, short, short);
  void keyPress(Window &, short, short, SDLKey, Uint16);

public:
  Workspace(System *);
  virtual ~Workspace();

  RootWindow * getRootWindow() const {
    return m_rootWindow;
  }

  void draw();
  void handleEvent(const SDL_Event &);
  void addToplevel(Toplevel *);

  virtual void map(Window * win, int x, int y, int & w, int & h);
};

} // namespace Sear

#endif // SEAR_GUI_WORKSPACE_H
