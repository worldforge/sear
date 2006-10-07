// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/Alert.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/box.hpp"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>

namespace Sear {

Alert::Alert(gcn::Container * parent, const std::string & msg)
{
  gcn::Color base = getBaseColor();
  base.a = 255;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  gcn::Label *l1 = new gcn::Label(msg);
  vbox->pack(l1);
  m_widgets.push_back(SPtr<gcn::Widget>(l1));

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &Alert::actionPressed));

  m_okButton = new gcn::Button("Okay");
  m_widgets.push_back(SPtr<gcn::Widget>(m_okButton));

  m_okButton->setFocusable(false);
  m_okButton->setEventId("ok");
  m_okButton->addActionListener(m_buttonListener);

  vbox->pack(m_okButton);

  add(vbox);

  resizeToContent();

  parent->add(this, parent->getWidth() / 2 - getWidth() / 2,
                    parent->getHeight() / 2 - getHeight() / 2);
}

Alert::~Alert()
{
  delete m_buttonListener;
}

void Alert::actionPressed(std::string event)
{
  gcn::BasicContainer * parent_widget = getParent();
  if (parent_widget == 0) {
    std::cout << "NO PARENT" << std::endl << std::flush;
    return;
  }
  gcn::Container * parent = dynamic_cast<gcn::Container *>(parent_widget);
  if (parent == 0) {
    std::cout << "WEIRD PARENT" << std::endl << std::flush;
    return;
  }
  parent->remove(this);
}

void Alert::logic()
{
  gcn::BasicContainer * parent_widget = getParent();
  if (parent_widget != 0) {
    gcn::Container * parent = dynamic_cast<gcn::Container *>(parent_widget);
    if (parent != 0) {
      parent->moveToTop(this);
    }
  }
  gcn::Window::logic();
}

} // namespace Sear
