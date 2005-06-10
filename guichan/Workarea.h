// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_WORKAREA_H
#define SEAR_GUICHAN_WORKAREA_H

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
class RootWidget;

class Gui : public gcn::Gui {
public:
  Gui();
  ~Gui();

  gcn::FocusHandler * getFocusHandler() const {
    return mFocusHandler;
  }
};

class Workarea {
protected:
  System * m_system;

  gcn::SDLInput * m_input;                 // Input driver
  gcn::OpenGLGraphics * m_graphics;        // Graphics driver
  gcn::OpenGLImageLoader * m_imageLoader;  // For loading images
  gcn::SDLImageLoader * m_hostImageLoader; // For loading images
  Gui * m_gui;                             // A Gui object - binds it together

  RootWidget * m_top;                  // Top level container widget
public:
  Workarea(System *);
  virtual ~Workarea();

  void resize(int width, int height);
  bool handleEvent(const SDL_Event &);
  void draw();

};

} // namespace Sear

#endif // SEAR_GUICHAN_WORKAREA_H
