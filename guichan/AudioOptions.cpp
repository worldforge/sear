// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2007 Simon Goodall

#include "guichan/AudioOptions.h"

#include "guichan/RootWidget.h"
#include "guichan/box.hpp"

#include <guichan.hpp>

#include <iostream>

namespace Sear {

AudioOptions::AudioOptions(RootWidget * top) : gcn::Window("Audio"),
                                               m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  gcn::Button * b = new gcn::Button("Apply");
  m_widgets.push_back(SPtr<gcn::Widget>(b));
  b->setActionEventId("apply");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  b = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(b));
  b->setActionEventId("close");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  vbox->pack(hbox);

  add(vbox);

  resizeToContent();
}

AudioOptions::~AudioOptions()
{
}

void AudioOptions::action(const gcn::ActionEvent &event)
{
  if (event.getId() == "apply") {
    std::cout << "Apply changes" << std::endl << std::flush;
  } else if (event.getId() == "close") {
    m_top->closeWindow(this);
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
