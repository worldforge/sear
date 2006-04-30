// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/Workarea.h"

#include "guichan/RootWidget.h"
#include "guichan/ConnectWindow.h"
#include "guichan/LoginWindow.h"
#include "guichan/CharacterWindow.h"
#include "guichan/Panel.h"
#include "guichan/StatusWindow.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/SpeechBubble.h"
#include "guichan/Alert.h"

#include "guichan/box.hpp"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "src/ActionHandler.h"
#include "src/FileHandler.h"
#include "src/Console.h"
#include "src/System.h"

#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>

#include <sage/GL.h>

#include <iostream>

namespace Sear {

static const bool debug = false;

static const std::string WORKSPACE = "workspace";

static const std::string WORKAREA_OPEN = "workarea_open";
static const std::string WORKAREA_CLOSE = "workarea_close";
static const std::string WORKAREA_ALERT = "workarea_alert";

static const std::string WORKAREA = "workarea";

static const std::string KEY_fixed_font = "fixed_font";
static const std::string KEY_fixed_font_characters = "fixed_font_characters";

// static const std::string DEFAULT_fixed_font = "${SEAR_INSTALL}/data/fixedfont.bmp";
// static const std::string DEFAULT_fixed_font_characters = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const std::string DEFAULT_fixed_font = "${SEAR_INSTALL}/data/rpgfont.png";
static const std::string DEFAULT_fixed_font_characters = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&`'*#=[]\"<>{}^~|_@$\\";

Gui::Gui()
{
}

Gui::~Gui()
{
}

Workarea::Workarea(System * s) : 
  m_system(s), 
  m_input(0),
  m_graphics(0),
  m_imageLoader(0),
  m_hostImageLoader(0),
  m_gui(0),
  m_panel(0),
  m_connectWindow(0),
  m_top(0)
{
}

void Workarea::init()
{
  m_imageLoader = new gcn::OpenGLImageLoader();
  m_hostImageLoader = new gcn::SDLImageLoader();

  // Set the loader that the OpenGLImageLoader should use to load images from
  // disk, as it can't do it itself, and then install the image loader into
  // guichan.
  m_imageLoader->setHostImageLoader(m_hostImageLoader);
  gcn::Image::setImageLoader(m_imageLoader);

  // Create the handler for OpenGL graphics.
  m_graphics = new gcn::OpenGLGraphics();

  // Tell it the size of our screen.
  Render * render = RenderSystem::getInstance().getRenderer();
  m_width = render->getWindowWidth();
  m_height = render->getWindowHeight();
  m_graphics->setTargetPlane(m_width, m_height);

  m_input = new gcn::SDLInput();

  m_top = new RootWidget();
  m_top->setDimension(gcn::Rectangle(0, 0, m_width, m_height));
  m_top->setOpaque(false);
  m_top->setFocusable(true);
  m_top->setTabInEnabled(false);

  m_gui = new Gui();
  m_gui->setGraphics(m_graphics);
  m_gui->setInput(m_input);
  m_gui->setTop(m_top);

  try {
    std::string font_path = m_fixed_font;
    m_system->getFileHandler()->expandString(font_path);

    gcn::ImageFont * font = new gcn::ImageFont(font_path, m_fixed_font_characters);
    // gcn::ImageFont * font = new gcn::ImageFont("/tmp/Font-Utopia.bmp", " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{!}~");
    gcn::Widget::setGlobalFont(font);
  } catch (...) {
    std::cerr << "Failed to load font " << m_fixed_font << std::endl << std::flush;
  }

  // gcn::Image * image = new gcn::Image("/tmp/gui-chan.bmp");
  // gcn::Icon * icon = new gcn::Icon(image);

  // m_top->add(icon, 10, 30);

  // LoginWindow * lw = new LoginWindow;
  // m_top->add(lw, m_width / 2 - lw->getWidth() / 2, m_height / 2 - lw->getHeight () / 2);
  // ConsoleWindow * cw = new ConsoleWindow;
  // m_top->add(cw, 4, m_height - cw->getHeight() - 4);

  // SpeechBubble * sp = new SpeechBubble;
  // sp->loadImages(std::vector<std::string>());
  // m_top->add(sp, 50, 50);

  SPtr<gcn::Window> con_w = SPtr<gcn::Window>(new ConnectWindow);
  m_top->add(con_w.get(), m_width / 2 - con_w->getWidth() / 2, m_height / 2 - con_w->getHeight () / 2);

  m_panel = SPtr<gcn::Window>(new Panel(m_top));
  // m_top->add(m_panel, 0, 0);

  m_windows["panel"] = m_panel;
  m_top->setWindowCoords(m_panel.get(), std::make_pair(0,0));
  m_windows["connect"] = con_w;
  m_windows["login"] = SPtr<gcn::Window>(new LoginWindow);
  m_windows["character"] = SPtr<gcn::Window>(new CharacterWindow);

  m_system->getActionHandler()->addHandler("connected", "/workarea_close connect");
  m_system->getActionHandler()->addHandler("connected", "/workarea_open login");
  m_system->getActionHandler()->addHandler("disconnected", "/workarea_alert Connection to server failed");
  m_system->getActionHandler()->addHandler("disconnected", "/workarea_open connect");

  m_system->getActionHandler()->addHandler("logged_in", "/workarea_close login");
  m_system->getActionHandler()->addHandler("logged_in", "/workarea_open character");

  m_system->getActionHandler()->addHandler("world_entered", "/workarea_close character");
  m_system->getActionHandler()->addHandler("world_entered", "/workarea_open panel");
  m_system->getActionHandler()->addHandler("avatar_failed", "/workarea_alert Unable to get character");

  m_system->getActionHandler()->addHandler("inventory_open", "/panel_toggle Inventory");

}

Workarea::~Workarea()
{
  delete m_gui;
  delete m_input;
  delete m_graphics;
  delete m_imageLoader;
  delete m_hostImageLoader;
  delete m_top;

  
}

void Workarea::registerCommands(Console * console)
{
  if (m_panel != 0) {
    dynamic_cast<Panel*>(m_panel.get())->registerCommands(console);
  }

  console->registerCommand(WORKAREA_OPEN, this);
  console->registerCommand(WORKAREA_CLOSE, this);
  console->registerCommand(WORKAREA_ALERT, this);
}

void Workarea::runCommand(const std::string & command, const std::string & args)
{
  if (command == WORKAREA_CLOSE) {
    if (debug) std::cout << "Got the workarea close command" << std::endl << std::flush;
    WindowDict::const_iterator I = m_windows.find(args);
    if (I != m_windows.end()) {
      SPtr<gcn::Window> win = I->second;
      assert(win != 0);
      if (win->getParent() != 0) {
        m_top->closeWindow(win.get());
      }
    } else {
      std::cerr << "Asked to close unknown window " << args
                << std::endl << std::flush;
    }
  }
  else if (command == WORKAREA_OPEN) {
    WindowDict::const_iterator I = m_windows.find(args);
    if (I != m_windows.end()) {
      SPtr<gcn::Window> win = I->second;
      assert(win != 0);
      if (win->getParent() == 0) {
        m_top->openWindow(win.get());
      }
    } else {
      std::cerr << "Asked to open unknown window " << args
                << std::endl << std::flush;
    }
  }
  else if (command == WORKAREA_ALERT) {
    std::cerr << "Asked to open ALERT" << args
              << std::endl << std::flush;
    Alert * al = new Alert(m_top, args);
    m_widgets.push_back(SPtr<gcn::Widget>(al));
    // m_top->openWindow(al);
  }
}

void Workarea::readConfig(varconf::Config & config)
{
  varconf::Variable temp;

  if (config.findItem(WORKAREA, KEY_fixed_font)) {
    temp = config.getItem(WORKAREA, KEY_fixed_font);
    m_fixed_font = (!temp.is_string()) ? (DEFAULT_fixed_font) : temp.as_string();
  } else {
    m_fixed_font = DEFAULT_fixed_font;
  }
  if (config.findItem(WORKAREA, KEY_fixed_font_characters)) {
    temp = config.getItem(WORKAREA, KEY_fixed_font_characters);
    m_fixed_font_characters = (!temp.is_string()) ? (DEFAULT_fixed_font_characters) : temp.as_string();
  } else {
    m_fixed_font_characters = DEFAULT_fixed_font_characters;
  }
}

void Workarea::writeConfig(varconf::Config & config)
{
  config.setItem(WORKAREA, KEY_fixed_font, m_fixed_font);
  config.setItem(WORKAREA, KEY_fixed_font_characters, m_fixed_font_characters);
}

void Workarea::resize()
{
  Render * render = RenderSystem::getInstance().getRenderer();
  int width = render->getWindowWidth(),
      height = render->getWindowHeight();
  m_graphics->setTargetPlane(width, height);
  m_top->resize(width, height, m_width, m_height);
  // m_top->setDimension(gcn::Rectangle(0, 0, width, height));
  m_width = width;
  m_height = height;
}

bool Workarea::handleEvent(const SDL_Event & event)
{
  gcn::FocusHandler * fh = m_gui->getFocusHandler();
  assert(fh != 0);

  gcn::Widget * focus = fh->getFocused();

  bool gui_has_mouse = m_top->childHasMouse();

  bool clear_focus = false;
  bool event_eaten = false;
  bool suppress = false;
  Panel * panel = dynamic_cast<Panel*>(m_panel.get());
  switch (event.type) {
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      // FIXME This should depend on whether the gui is visible.
      event_eaten = gui_has_mouse;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_RETURN) {
        if (panel != 0 && System::instance()->checkState(SYS_IN_WORLD) &&
            (focus == 0 || focus == m_top)) {
          suppress = panel->requestConsole();
        }
        event_eaten = true;
      } else if (event.key.keysym.sym == SDLK_SLASH) {
        if (panel != 0 && System::instance()->checkState(SYS_IN_WORLD) &&
            (focus == 0 || focus == m_top)) {
          panel->requestConsole();
        }
        event_eaten = true;
      } else if (event.key.keysym.sym == SDLK_ESCAPE) {
        if (panel != 0) {
          suppress = panel->dismissConsole();
        }
        event_eaten = true;
      } else {
        event_eaten = ((focus != 0) && (focus != m_top));
      }
    case SDL_KEYUP:
      event_eaten = ((focus != 0) && (focus != m_top));
      break;
    default:
      event_eaten = false;
      break;
  }

  if (!suppress) { m_input->pushInput(event); }

  if (clear_focus) {
    fh->focusNone();
  }

  focus = fh->getFocused();

  if ((focus != 0) && (focus != m_top)) {
    if (m_system->isMouselookEnabled()) {
      m_system->toggleMouselook();
    }
  }

  return event_eaten;
}

void Workarea::draw()
{
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(WORKSPACE));
  glLineWidth(1.f);

  m_gui->logic();
  m_gui->draw();

  glLineWidth(4.f);
}

} // namespace Sear
