// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_COMMAND_LINE_H
#define SEAR_GUICHAN_COMMAND_LINE_H

#include <guichan/widgets/textfield.hpp>

#include <sigc++/signal.h>

namespace Sear {

class CommandLine : public gcn::TextField
{
public:
  CommandLine(const std::string &txt) :
    TextField(txt) {}

protected:
  virtual void keyPressed(gcn::KeyEvent& key) {
    if (key.getKey().getValue() == gcn::Key::ENTER) {
      ReturnPressed.emit();
    }
    gcn::TextField::keyPressed(key);
  }
public:
  sigc::signal0<void> ReturnPressed;
};

} // namespace Sear

#endif // SEAR_GUICHAN_COMMAND_LINE_H
