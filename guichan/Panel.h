// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_PANEL_H
#define SEAR_GUICHAN_PANEL_H

#include "src/ConsoleObject.h"

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace gcn {

class Button;

} // namespace gcn

namespace Sear {

class Console;

class ActionListenerSigC;
class RootWidget;

class Panel : virtual public SigC::Object,
              public ConsoleObject,
              public gcn::Window {
protected:
  gcn::Button * m_inventoryButton;
  gcn::Button * m_optionsButton;

  RootWidget * m_top;

  gcn::Window * m_optionsWindow;

  ActionListenerSigC * m_buttonListener;
public:
  explicit Panel(RootWidget * top);
  virtual ~Panel();

  void registerCommands(Console *);
  virtual void runCommand(const std::string &, const std::string &);

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_PANEL_H
