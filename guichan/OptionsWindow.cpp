// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/OptionsWindow.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

OptionsWindow::OptionsWindow() : gcn::Window()
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  gcn::Box * hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &OptionsWindow::actionPressed));

  m_applyButton = new gcn::Button("Apply");
  m_applyButton->setEventId("apply");
  m_applyButton->addActionListener(m_buttonListener);
  hbox->pack(m_applyButton);

  m_closeButton = new gcn::Button("Close");
  m_closeButton->setEventId("close");
  m_closeButton->addActionListener(m_buttonListener);
  hbox->pack(m_closeButton);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();
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
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
