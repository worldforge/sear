// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _CONSOLEOBJECT_H_
#define _CONSOLEOBJECT_H_ 1

#include <string>

namespace Sear {

class ConsoleObject {
public:
  ConsoleObject() {}
  virtual ~ConsoleObject() {}
  virtual void runCommand(const std::string &command, const std::string &args) = 0;
};

} /* namespace Sear */
#endif /* _CONSOLEOBJECT_H_ */
