// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/ControlsOptions.h"

#include "guichan/RootWidget.h"
#include "guichan/box.hpp"

#include <guichan.hpp>

#include <iostream>

namespace Sear {

ControlsOptions::ControlsOptions(RootWidget * top) : gcn::Window("Controls"),
                                                     m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  gcn::Box * hbox = new gcn::HBox(6);

  gcn::Button * b = new gcn::Button("Apply");
  b->setEventId("apply");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  b = new gcn::Button("Close");
  b->setEventId("close");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();
}

ControlsOptions::~ControlsOptions()
{
}

void ControlsOptions::action(const std::string & event)
{
  if (event == "apply") {
    std::cout << "Apply changes" << std::endl << std::flush;
  } else if (event == "close") {
    m_top->closeWindow(this);
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
