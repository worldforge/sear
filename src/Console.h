// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Console.h,v 1.13 2004-04-17 15:55:45 simon Exp $

#ifndef SEAR_CONSOLE_H
#define SEAR_CONSOLE_H 1

#include <string>
#include <list>
#include <map>

#include "interfaces/ConsoleObject.h"


#define MAX_MESSAGES (5)
#define FONT_HEIGHT  (15)
#define LINE_SPACING (5)
#define CONSOLE_TEXT_OFFSET_X (5)
#define CONSOLE_TEXT_OFFSET_Y (5)

#define CONSOLE_SPEED (10)
#define CONSOLE_HEIGHT (100)

#define CONSOLE_PROMPT_STRING "> "
#define CONSOLE_CURSOR_STRING "_"

#define CONSOLE_MESSAGE 0x1
#define SCREEN_MESSAGE  0x2

namespace Sear {
// Forward Declarations
class ConsoleObject;
class System;

/**
 * The console class handles the onscreen console, the screen messages and keeps track of all the console commands in use.
 */ 
class Console : public ConsoleObject {
public:
  Console(System *system);
  ~Console();
  
  bool init();
  void shutdown();
 
  /**
   * Add a message to the console and/or screen message queue
   * message is the message string
   * type is where to display the messgae: CONSOLE_MESSAGE and/or SCREEN_MESSAGE
   * duration is how long the SCREEN_MESSAGE lasts before being removed
   */ 
  void pushMessage(const std::string &message, int type, int duration);
  /**
   * This method renders the console.
   * command is the current command string
   */ 
  void draw(const std::string &command);
  /**
   * Toggles whether the console is visible or not
   */ 
  void toggleConsole();
  /**
   * Returns whether the console is visible or not
   */ 
  bool consoleStatus() { return showConsole; }

  /**
   * Registers a command with the console
   * command is the command to register
   * object is the originating object
   */ 
  void registerCommand(const std::string &command, ConsoleObject *object);
  /**
   * This is the method the determines what object the pass the command onto
   * command is the command string to process
   */ 
  void runCommand(const std::string &command);

  /**
   * This is the ConsoleObject method.
   * command is the command to run
   * args is the commands arguments
   */ 
  void runCommand(const std::string &command, const std::string &args);

protected:
  // Pair used to assign a duration to the screen message
  typedef std::pair<std::string, unsigned int> screenMessage;

  /**
   *  This render the console messges plus the current command string
   *  command is the current command string
   */ 
  void renderConsoleMessages(const std::string &command);
  /**
   * This renders the screen messages
   */ 
  void renderScreenMessages();

  bool animateConsole; // Flag determining whether console is moving
  bool showConsole; // flag to say whether console is visible/useable or not
  int consoleHeight; // the height of the console. determined by number of messages allowed
  std::list<std::string> console_messages; // Current console messages
  std::list<screenMessage> screen_messages; // Current screen messages
  int panel_id; // The texture id of the console's panel texture
  System *_system;

  // Mapping of registered commands to assoicated object
  std::map<std::string, ConsoleObject*> _registered_commands;
 
  bool _initialised;
  
};

} /* namespace Sear */
#endif /* SEAR_CONSOLE_H */
