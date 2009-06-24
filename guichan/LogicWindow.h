// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#ifndef SEAR_GUICHAN_LOGICWINDOW_H
#define SEAR_GUICHAN_LOGICWINDOW_H 1

#include <guichan.hpp>

#include <sigc++/signal.h>

namespace Sear {

class LogicWindow : public gcn::Window
{
  public:
  LogicWindow(const std::string &caption) : gcn::Window(caption) {}
  virtual ~LogicWindow() {}

  virtual void logic() {
    Logic.emit();
    gcn::Window::logic();
  }

  sigc::signal<void> Logic;
};

} // namespace Sear

#endif // SEAR_GUICHAN_LOGICWINDOW_H
