// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/OptionsWindow.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/VideoOptions.h"
#include "guichan/RootWidget.h"
#include "guichan/box.hpp"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

OptionsWindow::OptionsWindow(RootWidget * top) : gcn::Window("Options"),
                                                 m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  gcn::Box * hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &OptionsWindow::actionPressed));

  gcn::Button * b = new gcn::Button("Video");
  b->setEventId("video");
  b->setFocusable(false);
  b->addActionListener(m_buttonListener);
  vbox->pack(b);

  b = new gcn::Button("Audio");
  b->setEventId("audio");
  b->setFocusable(false);
  b->addActionListener(m_buttonListener);
  vbox->pack(b);

  b = new gcn::Button("Controls");
  b->setEventId("controls");
  b->setFocusable(false);
  b->addActionListener(m_buttonListener);
  vbox->pack(b);

  b = new gcn::Button("Quit game");
  b->setEventId("quit");
  b->setFocusable(false);
  b->addActionListener(m_buttonListener);
  vbox->pack(b);

  m_applyButton = new gcn::Button("Apply");
  m_applyButton->setEventId("apply");
  m_applyButton->setFocusable(false);
  m_applyButton->addActionListener(m_buttonListener);
  hbox->pack(m_applyButton);

  m_closeButton = new gcn::Button("Close");
  m_closeButton->setEventId("close");
  m_closeButton->setFocusable(false);
  m_closeButton->addActionListener(m_buttonListener);
  hbox->pack(m_closeButton);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();

  m_windows["video"] = new VideoOptions(m_top);
}

OptionsWindow::~OptionsWindow()
{
}

void OptionsWindow::actionPressed(std::string event)
{
  if (event == "apply") {
    std::cout << "Apply changes" << std::endl << std::flush;
  } else if (event == "close") {
    std::cout << "Close window" << std::endl << std::flush;
    m_top->closeWindow(this);
  } else {
    std::map<std::string, gcn::Window *>::const_iterator I = m_windows.find(event);
    if (I == m_windows.end()) {
      std::cout << "Unknown options window " << event << " requested"
                << std::endl << std::flush;
      return;
    }
    gcn::Window * win = I->second;
    assert(win != 0);
    if (win->getParent() == 0) {
      m_top->openWindow(win);
    }
  }
}

#if 0
void OptionsWindow::openWindow(const std::string & name, gcn::Window * win)
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
#endif

} // namespace Sear
