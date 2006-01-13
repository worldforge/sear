// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_PANEL_H
#define SEAR_GUICHAN_PANEL_H

#include "src/ConsoleObject.h"

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

#include <map>
#include <string>

namespace gcn {

class Box;
class Button;

} // namespace gcn

namespace Sear {

class Console;

class ActionListenerSigC;
class RootWidget;
class ConsoleWindow;

class Panel : virtual public SigC::Object,
              public ConsoleObject,
              public gcn::Window {
public:
  typedef std::map<std::string, gcn::Button *> ButtonDict;
  typedef std::map<std::string, gcn::Window *> WindowDict;
  typedef std::map<std::string, std::pair<int, int> > CoordDict;
protected:
  gcn::Box * m_hbox;

  ButtonDict m_buttons;
  WindowDict m_windows;

  RootWidget * m_top;
  ConsoleWindow * m_console;

  ActionListenerSigC * m_buttonListener;

  void actionPressed(std::string);
public:
  explicit Panel(RootWidget * top);
  virtual ~Panel();

  void registerCommands(Console *);
  virtual void runCommand(const std::string &, const std::string &);

  void addWindow(gcn::Window * window);
  bool requestConsole();
  bool dismissConsole();
};

} // namespace Sear

#endif // SEAR_GUICHAN_PANEL_H
