// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/Panel.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/OptionsWindow.h"
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

Panel::Panel(RootWidget * top) : gcn::Window(), m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setTitleBarHeight(0);
  setMovable(false);

  m_optionsWindow = new OptionsWindow;

  // setOpaque(true);

  gcn::Box * hbox = new gcn::HBox(6);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(SigC::slot(*this, &Panel::actionPressed));

  m_optionsButton = new gcn::Button("Options");
  m_optionsButton->setEventId("options");
  m_optionsButton->addActionListener(m_buttonListener);
  hbox->pack(m_optionsButton);

  m_inventoryButton = new gcn::Button("Inventory");
  m_inventoryButton->setEventId("inventory");
  m_inventoryButton->addActionListener(m_buttonListener);
  hbox->pack(m_inventoryButton);

  setContent(hbox);

  resizeToContent();
}

Panel::~Panel()
{
}

void Panel::actionPressed(std::string event)
{
  Render * render = RenderSystem::getInstance().getRenderer();
  int width = render->getWindowWidth(),
      height = render->getWindowHeight();

  if (event == "options") {
    std::cout << "Open options window" << std::endl << std::flush;
    if (m_optionsWindow->getParent() == 0) {
      m_top->add(m_optionsWindow, width / 2 - m_optionsWindow->getWidth() / 2,
                                  height / 2 - m_optionsWindow->getHeight() / 2);
    } else {
      m_top->remove(m_optionsWindow);
    }
  } else if (event == "inventory") {
    std::cout << "Open inventory window" << std::endl << std::flush;
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
