// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/Panel.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/ConsoleWindow.h"
#include "guichan/OptionsWindow.h"
#include "guichan/RootWidget.h"
#include "guichan/box.hpp"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "src/System.h"
#include "src/Console.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

static std::string PANEL_OPEN = "panel_open";
static std::string PANEL_CLOSE = "panel_close";

Panel::Panel(RootWidget * top) : gcn::Window(), m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setTitleBarHeight(0);
  setMovable(false);

  // setOpaque(true);

  m_hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &Panel::actionPressed));

  setContent(m_hbox);

  addWindow("options", new OptionsWindow);
  addWindow("inventory", new OptionsWindow);

  Render * render = RenderSystem::getInstance().getRenderer();
  int height = render->getWindowHeight();

  gcn::Window * con = new ConsoleWindow;
  addWindow("chat", con);
  m_coords["chat"] = std::make_pair(4, height - con->getHeight() / 2 - 4);
}

Panel::~Panel()
{
}

void Panel::registerCommands(Console * console)
{
  console->registerCommand(PANEL_OPEN, this);
  console->registerCommand(PANEL_CLOSE, this);
}

void Panel::runCommand(const std::string & command, const std::string & args)
{
  if (command == PANEL_CLOSE) {
    std::cout << "Got the panel close command" << std::endl << std::flush;
    WindowDict::const_iterator I = m_windows.find(args);
    if (I != m_windows.end()) {
      gcn::Window * win = I->second;
      assert(win != 0);
      if (win->getParent() != 0) {
        closeWindow(args, win);
      }
    }
  }
  else if (command == PANEL_OPEN) {
    WindowDict::const_iterator I = m_windows.find(args);
    if (I != m_windows.end()) {
      gcn::Window * win = I->second;
      assert(win != 0);
      if (win->getParent() == 0) {
        openWindow(args, win);
      }
    }
  }
}

void Panel::actionPressed(std::string event)
{
  WindowDict::const_iterator I = m_windows.find(event);
  if (I != m_windows.end()) {
    gcn::Window * win = I->second;
    assert(win != 0);
    if (win->getParent() == 0) {
      openWindow(event, win);
    } else {
      closeWindow(event, win);
    }
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

void Panel::openWindow(const std::string & name, gcn::Window * win)
{
  int x, y;
  Render * render = RenderSystem::getInstance().getRenderer();
  int width = render->getWindowWidth(),
      height = render->getWindowHeight();

  CoordDict::const_iterator I = m_coords.find(name);

  if (I != m_coords.end()) {
    x = std::max(std::min(I->second.first, width - win->getWidth()), 0);
    y = std::max(std::min(I->second.second, height - win->getHeight()), 0);
  } else {
    x = width / 2 - win->getWidth() / 2;
    y = height / 2 - win->getHeight() / 2;
  }

  m_top->add(win, x, y);
}

void Panel::closeWindow(const std::string & name, gcn::Window * win)
{
  m_coords[name] = std::make_pair(win->getX(), win->getY());
  m_top->remove(win);
}

void Panel::addWindow(const std::string & name, gcn::Window * window)
{
  gcn::Button * button = new gcn::Button(name);
  button->setEventId(name);
  button->addActionListener(m_buttonListener);
  m_hbox->pack(button);

  m_buttons[name] = button;
  m_windows[name] = window;

  resizeToContent();
}

} // namespace Sear
