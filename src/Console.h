// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall, University of Southampton

// $Id: Console.h,v 1.16 2007-02-12 21:44:00 simon Exp $

#ifndef SEAR_CONSOLE_H
#define SEAR_CONSOLE_H 1

#include <string>
#include <list>
#include <map>
// Note: I don't like this very much, but it is needed for the KeySyms
#include <SDL/SDL.h>

#include "interfaces/ConsoleObject.h"


#define MAX_MESSAGES (5)
#define FONT_HEIGHT  (15)
#define LINE_SPACING (5)
#define CONSOLE_TEXT_OFFSET_X (5)
#define CONSOLE_TEXT_OFFSET_Y (5)

#define CONSOLE_SPEED (10)
#define CONSOLE_HEIGHT (100)

#define CONSOLE_PROMPT_STRING "> "
#define CONSOLE_PROMPT_LENGTH 2
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
  bool isInitialised() const { return m_initialised; }
 
  /**
   * Add a message to the console and/or screen message queue
   * message is the message string
   * type is where to display the messgae: CONSOLE_MESSAGE and/or SCREEN_MESSAGE
   * duration is how long the SCREEN_MESSAGE lasts before being removed
   */ 
  void pushMessage(const std::string &message, int type, int duration);
  /**
   * This method renders the console.
   */ 
  void draw(void);
  /**
   * Toggles whether the console is visible or not
   */ 
  void toggleConsole();
  /**
   * Returns whether the console is visible or not
   */ 
  bool consoleStatus() const { return m_showConsole; }
  
  /**
   * This function will change the content of the console according to the key that was  pressed.
   **/
  void vHandleInput(const SDLKey &KeySym, const Uint16 &UnicodeCharacter);
  
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
   */ 
  void renderConsoleMessages(void);
  /**
   * This renders the screen messages
   */ 
  void renderScreenMessages();

  bool m_animateConsole; // Flag determining whether console is moving
  bool m_showConsole; // flag to say whether console is visible/useable or not
  int m_consoleHeight; // the height of the console. determined by number of messages allowed
  std::list<std::string> m_console_messages; // Current console messages
  std::list<screenMessage> m_screen_messages; // Current screen messages
  int m_panel_id; // The texture id of the console's panel texture
  int m_panel_state;
  int m_font_state;
  System *m_system;

  // Mapping of registered commands to assoicated object
  std::map<std::string, ConsoleObject*> m_registered_commands;
  
  /// Storage of the least recent commands in reverse order
  std::list< std::string > m_CommandHistory;
  std::list< std::string >::iterator m_CommandHistoryIterator;
  
  /// Command currently being edited in the console
  std::string m_Command;
  
  /// The current caret position, 0-based.
  unsigned int m_CaretPosition;
  
  /// A map identifying valid token starts and referencing those
  std::multimap< std::string, std::string > m_CommandStarts;
  
  /// Whether the tab key was the last key pressed once.
  bool m_bTabOnce;
 
  bool m_initialised;
  
};

} /* namespace Sear */
#endif /* SEAR_CONSOLE_H */
