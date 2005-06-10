// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_CONSOLE_WINDOW_H
#define SEAR_GUICHAN_CONSOLE_WINDOW_H

#include <guichan/widgets/window.hpp>

namespace Sear {

class ConsoleWindow : public gcn::Window {
public:
  ConsoleWindow();
  virtual ~ConsoleWindow();
};

} // namespace Sear

#endif // SEAR_GUICHAN_CONSOLE_WINDOW_H
