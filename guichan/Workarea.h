// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_WORKAREA_H
#define SEAR_GUICHAN_WORKAREA_H

#include "src/ConsoleObject.h"

#include <guichan.hpp>

#include <SDL/SDL.h>

namespace gcn {
  class SDLInput;
  class OpenGLGraphics;
  class OpenGLImageLoader;
  class SDLImageLoader;
}

namespace Sear {

class System;
class Console;

class RootWidget;
class Panel;

class Gui : public gcn::Gui {
public:
  Gui();
  ~Gui();

  gcn::FocusHandler * getFocusHandler() const {
    return mFocusHandler;
  }
};

class Workarea : public ConsoleObject {
protected:
  System * m_system;
  int m_width, m_height;

  gcn::SDLInput * m_input;                 // Input driver
  gcn::OpenGLGraphics * m_graphics;        // Graphics driver
  gcn::OpenGLImageLoader * m_imageLoader;  // For loading images
  gcn::SDLImageLoader * m_hostImageLoader; // For loading images
  Gui * m_gui;                             // A Gui object - binds it together

  Panel * m_panel;

  RootWidget * m_top;                  // Top level container widget
public:
  Workarea(System *);
  virtual ~Workarea();

  void registerCommands(Console *);
  virtual void runCommand(const std::string &, const std::string &);

  void resize(int width, int height);
  bool handleEvent(const SDL_Event &);
  void draw();

};

} // namespace Sear

#endif // SEAR_GUICHAN_WORKAREA_H
