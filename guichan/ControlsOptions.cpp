// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch
// Copyright (C) 2007 Simon Goodall

#include "guichan/ControlsOptions.h"

#include "guichan/RootWidget.h"
#include "guichan/Alert.h"
#include "guichan/box.hpp"

#include "src/Bindings.h"

#include <guichan.hpp>

#include <varconf/config.h>

#include <iostream>
#include <sstream>

namespace Sear {

void ControlsOptions::readBindings()
{
  varconf::Config * binding = Bindings::getBindings();
  if (binding != 0) {
    std::stringstream key_data;
    binding->writeToStream(key_data, (varconf::Scope)(varconf::USER | varconf::GLOBAL | varconf::INSTANCE));
    m_controlText->setText(key_data.str());
  }
}

ControlsOptions::ControlsOptions(RootWidget * top) : gcn::Window("Controls"),
                                                     m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  m_controlText = new gcn::TextBox;
  m_widgets.push_back(SPtr<gcn::Widget>(m_controlText));
  m_controlText->setEditable(false);
  m_controlText->setOpaque(false);
  m_controlText->setFocusable(false);
  m_controlText->setEnabled(false);
  gcn::ScrollArea * control_scroll = new gcn::ScrollArea(m_controlText,
                                                         gcn::ScrollArea::SHOW_AUTO,
                                                         gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(control_scroll));
  control_scroll->setWidth(500);
  control_scroll->setHeight(300);
  control_scroll->setBorderSize(1);
  vbox->pack(control_scroll);

  readBindings();

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  gcn::Label * l = new gcn::Label("Key");
  m_widgets.push_back(SPtr<gcn::Widget>(l));
  hbox->pack(l);

  m_key = new gcn::TextField("          ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_key));
  m_key->setText("");
  hbox->pack(m_key);

  l = new gcn::Label("Action");
  m_widgets.push_back(SPtr<gcn::Widget>(l));
  hbox->pack(l);

  m_action = new gcn::TextField("                                        ");
  m_widgets.push_back(SPtr<gcn::Widget>(m_action));
  m_action->setText("");
  hbox->pack(m_action);

  gcn::Button * b = new gcn::Button("Bind");
  m_widgets.push_back(SPtr<gcn::Widget>(b));
  b->setActionEventId("bind");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  vbox->pack(hbox);

  hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  b = new gcn::Button("Apply");
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

ControlsOptions::~ControlsOptions()
{
}

void ControlsOptions::action(const gcn::ActionEvent &actionEvent)
{
  const std::string &event = actionEvent.getId();
  if (event == "apply") {
    std::cout << "Apply changes" << std::endl << std::flush;
  } else if (event == "bind") {
    const std::string & key = m_key->getText();
    const std::string & action = m_action->getText();
    if (key.empty() || action.empty()) {
      new Alert(m_top, "No key or action specified.");
    } else {
      Bindings::bind(key, action);
      m_key->setText("");
      m_action->setText("");
      readBindings();
    }
  } else if (event == "close") {
    m_top->closeWindow(this);
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
