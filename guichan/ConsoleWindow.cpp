// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/ConsoleWindow.h"
#include "guichan/box.hpp"

#include <guichan.hpp>

#include <iostream>

namespace Sear {

ConsoleWindow::ConsoleWindow() : gcn::Window("Console")
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  gcn::Box * vbox = new gcn::VBox(6);

  gcn::TextBox * textBox = new gcn::TextBox("Multiline\nText box");
  textBox->setEditable(false);
  textBox->setOpaque(false);
  textBox->setFocusable(false);
  textBox->setBackgroundColor(gcn::Color(0,0,0,0));
  gcn::ScrollArea * textBoxScrollArea = new gcn::ScrollArea(textBox,
                                            gcn::ScrollArea::SHOW_NEVER,
                                            gcn::ScrollArea::SHOW_ALWAYS);
  textBoxScrollArea->setWidth(400);
  textBoxScrollArea->setHeight(100);
  textBoxScrollArea->setBorderSize(1);

  vbox->pack(textBoxScrollArea);

  gcn::TextField * entry = new gcn::TextField();
  entry->setWidth(400);

  vbox->pack(entry);

  setContent(vbox);

  resizeToContent();
}

ConsoleWindow::~ConsoleWindow()
{
}

} // namespace Sear
