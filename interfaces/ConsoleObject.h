// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ConsoleObject.h,v 1.2 2006-04-26 14:38:59 simon Exp $

#ifndef SEAR_CONSOLEOBJECT_H
#define SEAR_CONSOLEOBJECT_H 1

#include <string>

namespace Sear {
/**
 * The ConsoleObject is a interface used to allow objects to register commands
 * with the console. Any object wishing to register a command, must implement 
 *  this interface.
 */ 
class ConsoleObject {
public:
  /**
   * Default constructor
   */ 	
  ConsoleObject() {}

  /**
   * Default destructor
   */ 
  virtual ~ConsoleObject() {}

  /**
   * This is the function that needs to be extended to use the console.
   * command is a command that has been previously registered with the console
   * args is the argument string that has been provided for the command
   */ 
  virtual void runCommand(const std::string &command, const std::string &args) = 0;
};

} /* namespace Sear */
#endif /* SEAR_CONSOLEOBJECT_H */
