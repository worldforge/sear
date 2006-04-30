// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch

#include "guichan/TaskWindow.h"

#include "guichan/box.hpp"
#include "guichan/bar.hpp"

#include "src/System.h"
#include "src/Character.h"

#include <Eris/Entity.h>
#include <Eris/Task.h>

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <iostream>
#include <cassert>

namespace Sear {

TaskWindow::TaskWindow(Eris::Entity * e) : m_entity(e)
{
  assert(e != 0);

  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  // setTitleBarHeight(0);
  setMovable(false);

  // setOpaque(true);

  m_vbox = new gcn::VBox(2);

  m_progressBar = new gcn::Bar;
  m_progressBar->setWidth(100);
  m_progressBar->setHeight(6);
  m_progressBar->setValue(0.0);
  m_progressBar->setForegroundColor(gcn::Color(0xaaaaaa));
  m_progressBar->setBaseColor(gcn::Color(0,0,0,25));
  m_vbox->pack(m_progressBar);

  setContent(m_vbox);
  resizeToContent();
}

TaskWindow::~TaskWindow()
{
}

void TaskWindow::logic()
{
  if (m_entity) {
    const Eris::TaskArray & tasks = m_entity->getTasks();
    if (!tasks.empty()) {
      Eris::Task * t = tasks.front();
      std::cout << "GOT TASK" << t->name() << ":" << t->progress() << std::endl << std::flush;
      setCaption(t->name());
      m_progressBar->setValue(t->progress());
    }
  }
  gcn::Window::logic();
}

} // namespace Sear
