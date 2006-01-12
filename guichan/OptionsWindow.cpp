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

#include "src/System.h"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

OptionsWindow::OptionsWindow(RootWidget * top) : gcn::Window("System"),
                                                 m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &OptionsWindow::actionPressed));

  gcn::Box * vbox = new gcn::VBox(6);

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

  b = new gcn::Button("Return to game");
  b->setEventId("close");
  b->setFocusable(false);
  b->addActionListener(m_buttonListener);
  vbox->pack(b);

  setContent(vbox);

  resizeToContent();

  m_windows["video"] = new VideoOptions(m_top);
}

OptionsWindow::~OptionsWindow()
{
}

void OptionsWindow::actionPressed(std::string event)
{
  if (event == "quit") {
    std::cout << "Quit game" << std::endl << std::flush;
    System::instance()->runCommand("/quit");
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

} // namespace Sear
