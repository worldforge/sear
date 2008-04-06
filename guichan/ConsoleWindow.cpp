// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2007 Simon Goodall

#include "guichan/key.hpp"
#include "guichan/ConsoleWindow.h"
#include "guichan/CommandLine.h"
#include "guichan/box.hpp"

#include "src/System.h"

#include <guichan.hpp>

#include <sigc++/signal.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

void ConsoleWindow::pushMessage(const std::string & msg, int, int)
{
//  std::cout << "Got console message " << msg << std::endl << std::flush;
  m_textBox->addRow(msg);
  m_textBox->setCaretRow(m_textBox->getNumberOfRows() -1);
  m_textBox->scrollToCaret();
}

ConsoleWindow::ConsoleWindow() : gcn::Window("Console")
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  m_textBox = new gcn::TextBox;
  m_widgets.push_back(SPtr<gcn::Widget>(m_textBox));
  m_textBox->setEditable(false);
  m_textBox->setOpaque(false);
  m_textBox->setFocusable(false);
  m_textBox->setEnabled(false);
  m_textBox->setBackgroundColor(gcn::Color(0,0,0,0));
  m_scrollArea = new gcn::ScrollArea(m_textBox,
                                       gcn::ScrollArea::SHOW_AUTO,
                                       gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(m_scrollArea));
  m_scrollArea->setWidth(400);
  m_scrollArea->setHeight(100);
  m_scrollArea->setBorderSize(1);

  vbox->pack(m_scrollArea);

  m_entry = new CommandLine;
  m_widgets.push_back(SPtr<gcn::Widget>(m_entry));
  m_entry->setWidth(400);
  m_entry->setTabInEnabled(false);
  m_entry->setTabOutEnabled(false);
  m_entry->ReturnPressed.connect(sigc::mem_fun(*this, &ConsoleWindow::lineEntered));

  vbox->pack(m_entry);

  add(vbox);

  resizeToContent();

  System::instance()->pushedMessage.connect(sigc::mem_fun(*this, &ConsoleWindow::pushMessage));
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::lineEntered()
{
//  std::cout << "Line entered" << std::endl << std::flush;
  if (m_entry->getText() == "") {
    _getFocusHandler()->focusNone();
  } else {
    System::instance()->runCommand(m_entry->getText());
    m_entry->setText("");
  }
}

bool ConsoleWindow::requestConsoleFocus()
{
  if (!m_entry->isFocused()) {
//    std::cout << "Request Focus" << std::endl << std::flush;
    m_entry->requestFocus();
    return true;
  }
  return false;
}

bool ConsoleWindow::dismissConsoleFocus()
{
  if (m_entry->isFocused()) {
    _getFocusHandler()->focusNone();
    return true;
  }
  return false;
}

void ConsoleWindow::logic()
{
  if (m_entry->isFocused()) {
    gcn::Color base = getBaseColor();
    base.a = 128;
    setBaseColor(base);
    m_scrollArea->setBackgroundColor(gcn::Color(0xffffff));
    m_entry->setBackgroundColor(gcn::Color(0xffffff));
  } else {
    gcn::Color base = getBaseColor();
    base.a = 31;
    setBaseColor(base);
    m_scrollArea->setBackgroundColor(gcn::Color(255,255,255,127));
    m_entry->setBackgroundColor(gcn::Color(0,0,0,0));
  }
  gcn::Window::logic();
}

} // namespace Sear
