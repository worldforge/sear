// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_CONSOLE_WINDOW_H
#define SEAR_GUICHAN_CONSOLE_WINDOW_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace gcn {
class TextBox;
}

namespace Sear {

class CommandLine;

class ConsoleWindow : public gcn::Window, virtual public SigC::Object {
protected:
  gcn::TextBox * m_textBox;
  CommandLine * m_entry;

  void pushMessage(const std::string &, int, int);
public:
  ConsoleWindow();
  virtual ~ConsoleWindow();

  void lineEntered();
  bool requestConsoleFocus();
  bool dismissConsoleFocus();
};

} // namespace Sear

#endif // SEAR_GUICHAN_CONSOLE_WINDOW_H
