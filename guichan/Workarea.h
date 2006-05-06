// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_WORKAREA_H
#define SEAR_GUICHAN_WORKAREA_H

#include <sigc++/trackable.h>

#include "src/ConsoleObject.h"

#include <guichan.hpp>

#include <SDL/SDL.h>

#include <common/SPtr.h>

namespace varconf {
  class Config;
}

namespace gcn {
  class SDLInput;
  class OpenGLGraphics;
  class OpenGLImageLoader;
  class SDLImageLoader;
}

namespace Sear {

class System;
class Console;

class Panel;
class ConnectWindow;
class RootWidget;

class Gui : public gcn::Gui {
public:
  Gui();
  ~Gui();

  gcn::FocusHandler * getFocusHandler() const {
    return mFocusHandler;
  }
};

class Workarea : public ConsoleObject, public sigc::trackable {
public:
  typedef std::map<std::string, SPtr<gcn::Button> > ButtonDict;
  typedef std::map<std::string, SPtr<gcn::Window> > WindowDict;
  typedef std::map<std::string, std::pair<int, int> > CoordDict;
protected:
  std::list<SPtr<gcn::Widget> > m_widgets;
  System * m_system;
  int m_width, m_height;
  std::string m_fixed_font;
  std::string m_fixed_font_characters;

  gcn::SDLInput * m_input;                 // Input driver
  gcn::OpenGLGraphics * m_graphics;        // Graphics driver
  gcn::OpenGLImageLoader * m_imageLoader;  // For loading images
  gcn::SDLImageLoader * m_hostImageLoader; // For loading images
  Gui * m_gui;                             // A Gui object - binds it together

  ButtonDict m_buttons;
  WindowDict m_windows;

  SPtr<gcn::Window> m_panel;
  SPtr<gcn::Window> m_connectWindow;

  RootWidget * m_top;                  // Top level container widget
public:
  Workarea(System *);
  virtual ~Workarea();

  void init();

  void registerCommands(Console *);
  virtual void runCommand(const std::string &, const std::string &);
  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config);

  void contextCreated();
  void contextDestroyed(bool check);

  void resize();
  bool handleEvent(const SDL_Event &);
  void draw();

};

} // namespace Sear

#endif // SEAR_GUICHAN_WORKAREA_H
