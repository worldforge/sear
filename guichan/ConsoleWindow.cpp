// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/ConsoleWindow.h"
#include "guichan/box.hpp"

#include "src/System.h"

#include <guichan.hpp>

#include <sigc++/signal.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

class CommandLine : public gcn::TextField
{
protected:
  virtual void keyPress(const gcn::Key& key) {
    if (key.getValue() == gcn::Key::ENTER) {
      ReturnPressed.emit();
    }
    gcn::TextField::keyPress(key);
  }
public:
  SigC::Signal0<void> ReturnPressed;
};

void ConsoleWindow::pushMessage(const std::string & msg, int, int)
{
  std::cout << "Got console message " << msg << std::endl << std::flush;
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

  m_textBox = new gcn::TextBox;
  m_textBox->setEditable(false);
  m_textBox->setOpaque(false);
  m_textBox->setFocusable(false);
  m_textBox->setEnabled(false);
  m_textBox->setBackgroundColor(gcn::Color(0,0,0,0));
  gcn::ScrollArea * textBoxScrollArea = new gcn::ScrollArea(m_textBox,
                                            gcn::ScrollArea::SHOW_AUTO,
                                            gcn::ScrollArea::SHOW_ALWAYS);
  textBoxScrollArea->setWidth(400);
  textBoxScrollArea->setHeight(100);
  textBoxScrollArea->setBorderSize(1);

  vbox->pack(textBoxScrollArea);

  m_entry = new CommandLine;
  m_entry->setWidth(400);
  m_entry->setTabInEnabled(false);
  m_entry->setTabOutEnabled(false);
  m_entry->ReturnPressed.connect(SigC::slot(*this, &ConsoleWindow::lineEntered));

  vbox->pack(m_entry);

  setContent(vbox);

  resizeToContent();

  System::instance()->pushedMessage.connect(SigC::slot(*this, &ConsoleWindow::pushMessage));
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::lineEntered()
{
  std::cout << "Line entered" << std::endl << std::flush;
  if (m_entry->getText() == "") {
    _getFocusHandler()->focusNone();
  } else {
    System::instance()->runCommand(m_entry->getText());
    m_entry->setText("");
  }
}

bool ConsoleWindow::requestConsoleFocus()
{
  if (!m_entry->hasFocus()) {
    std::cout << "Request Focus" << std::endl << std::flush;
    m_entry->requestFocus();
    return true;
  }
  return false;
}

bool ConsoleWindow::dismissConsoleFocus()
{
  if (m_entry->hasFocus()) {
    _getFocusHandler()->focusNone();
    return true;
  }
  return false;
}

} // namespace Sear
