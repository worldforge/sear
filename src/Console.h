// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CONSOLE_H_
#define _CONSOLE_H_ 1

//#include <stdlib.h>
//#include <stdio.h>
#include <string>
#include <list>
#include <map>

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

#include "ConsoleObject.h"

namespace Sear {

class ConsoleObject;
class System;
class Render;

class Console : public ConsoleObject {
public:
  Console(System *system) :
    animateConsole(0),
    showConsole(0),
    consoleHeight(0),
    console_messages(std::list<std::string>()),
    screen_messages(std::list<screenMessage>()),
    _system(system),
    _renderer(NULL)
  {
  }
  ~Console() {}
  bool init();
  void shutdown();
 
  void pushMessage(const std::string &, int, int);
  void draw(const std::string &);
  void toggleConsole();
  bool consoleStatus() { return showConsole; }

  void registerCommand(const std::string &command, ConsoleObject *object);
  void runCommand(const std::string &command);
  void runCommand(const std::string &command, const std::string &args);

protected:
  typedef std::pair<std::string, unsigned int> screenMessage;

  void renderConsoleMessages(const std::string &);
  void renderScreenMessages();

  bool animateConsole;
  bool showConsole;
  int consoleHeight;
  std::list<std::string> console_messages;
  std::list<screenMessage> screen_messages;
  int panel_id;
  System *_system;
  Render *_renderer;

  std::map<std::string, ConsoleObject*> _registered_commands;
 
  static const char * const TOGGLE_CONSOLE = "toggle_console";
  static const char * const LIST_CONSOLE_COMMANDS = "list_commands";
  
};

} /* namespace Sear */
#endif /* _CONSOLE_H_ */
