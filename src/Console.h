// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CONSOLE_H_
#define _CONSOLE_H_ 1

//#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <list>

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

//#define MESSAGE_LIFE 5000


class System;
class Render;

class Console {
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
};

#endif /* _CONSOLE_H_ */
