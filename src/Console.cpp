// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: Console.cpp,v 1.30 2004-04-27 15:07:02 simon Exp $
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "common/Utility.h"
#include "common/Log.h"

#include "Bindings.h"
#include "Console.h"
#include "System.h"
#include "Graphics.h"
#include "Render.h"
#include "FileHandler.h"

#include "renderers/RenderSystem.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {
	
static const std::string TOGGLE_CONSOLE = "toggle_console";
static const std::string LIST_CONSOLE_COMMANDS = "list_commands";

static const std::string UI = "ui";
static const std::string PANEL = "panel";
static const std::string FONT = "font";

static const std::string SAY = "say";
static const std::string CMD_SAY = "/say ";

Console::Console(System *system) :
  animateConsole(0),
  showConsole(0),
  consoleHeight(0),
  console_messages(std::list<std::string>()),
  screen_messages(std::list<screenMessage>()),
  panel_id(0),
  _system(system),
  _initialised(false)
{
  assert((system != NULL) && "System is NULL");
}

Console::~Console() {
  if (_initialised) shutdown();
}

bool Console::init() {
  if (_initialised) shutdown();
  // Register console commands
  registerCommand(TOGGLE_CONSOLE, this);
  registerCommand(LIST_CONSOLE_COMMANDS, this);
  //Makes sure at least one key is bound to the console
  Bindings::bind("backquote", "/" + std::string(TOGGLE_CONSOLE));
  _initialised = true;
  return true;
}

void Console::shutdown() {
  if (debug) Log::writeLog("Shutting down console.", Log::LOG_DEFAULT);
  while (!_registered_commands.empty()) _registered_commands.erase(_registered_commands.begin());
  while (!console_messages.empty()) console_messages.erase(console_messages.begin());
  while (!screen_messages.empty()) screen_messages.erase(screen_messages.begin());
  _initialised = false;
}

void Console::pushMessage(const std::string &message, int type, int duration) {
  assert ((_initialised == true) && "Console not initialised");
  // Is this a screen message
  if (type & SCREEN_MESSAGE) {	
    //If we have reached our message limit, remove the oldest message regardless of duration
    if (screen_messages.size() >= MAX_MESSAGES) screen_messages.erase(screen_messages.begin());
    screen_messages.push_back(screenMessage(message, _system->getTime() + duration));
  }
  // Is this a console message?
  if (type & CONSOLE_MESSAGE) {
    //If we have reached our message limit, remove the oldest message regardless of duration
    if (console_messages.size() >= MAX_MESSAGES) console_messages.erase(console_messages.begin());
    console_messages.push_back(message);
  }
}

void Console::draw(const std::string &command) {
  assert ((_initialised == true) && "Console not initialised");
  // If we are animating and putting console into visible state,
  //  the raise height a tad
  if (animateConsole && showConsole) {
    consoleHeight += CONSOLE_SPEED;
    // Have we reached full height?
    if (consoleHeight >= CONSOLE_HEIGHT) {
      // Disable animation
      consoleHeight = CONSOLE_HEIGHT;
      animateConsole = 0;
    }
    renderConsoleMessages(command);
  // Ife we are animating and putting console into hidden state,
  //  the lower height a tad
  } else if (animateConsole && !showConsole) {
    consoleHeight -= CONSOLE_SPEED;
    // Have we become visible?
    if (consoleHeight <= 0) {
      //Disable animation
      consoleHeight = 0;
      animateConsole = 0;
    }
    renderConsoleMessages(command);
  // Else are we just plain visible?    
  } else if (showConsole) {
    renderConsoleMessages(command);
  }
  //Screen messages are always visible
  renderScreenMessages();
}

void Console::renderConsoleMessages(const std::string &command) {
  assert ((_initialised == true) && "Console not initialised");
  Render *renderer = _system->getGraphics()->getRender();
  if (!renderer) {
    Log::writeLog("Console: Error - Renderer object not created", Log::LOG_ERROR);
    return;
  }
  std::list<std::string>::const_iterator I;
  int i;
  //Render console panel
  int consoleOffset = CONSOLE_HEIGHT - consoleHeight;
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(PANEL));
  //Make panel slightly transparent
  renderer->setColour(0.0f, 0.0f, 1.0f, 0.85f);
  if (panel_id == 0) panel_id = RenderSystem::getInstance().requestTexture(PANEL);
  renderer->drawTextRect(0, 0, renderer->getWindowWidth(), consoleHeight, panel_id);
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(FONT));
  renderer->setColour(1.0f, 1.0f, 0.0f, 1.0f);
  //Render console messges
  for (I = console_messages.begin(), i = 0; I != console_messages.end(); ++I, ++i) {
    int j = console_messages.size() - i;
    renderer->print(CONSOLE_TEXT_OFFSET_X, CONSOLE_TEXT_OFFSET_Y + j * FONT_HEIGHT - consoleOffset, (char*)(*I).c_str(), 0);
  }
  //Render current command string
  std::string str = CONSOLE_PROMPT_STRING + command + CONSOLE_CURSOR_STRING;
  renderer->print(CONSOLE_TEXT_OFFSET_X, CONSOLE_TEXT_OFFSET_Y - consoleOffset, const_cast<char *>(str.c_str()), 0);
}

void Console::renderScreenMessages() {
  assert ((_initialised == true) && "Console not initialised");
  Render *renderer = _system->getGraphics()->getRender();
  if (screen_messages.empty()) return;	
  std::list<screenMessage>::const_iterator I;
  int i;
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(FONT));
  renderer->setColour(1.0f, 1.0f, 0.0f, 1.0f);
  // Get screen height so we can calculate offset correctly
  int height = renderer->getWindowHeight();
  //Get time so we can remove expired messages
  unsigned int current_time = _system->getTime();
  //Render messges
  for (I = screen_messages.begin(), i = 0; I != screen_messages.end(); ++I, ++i) {
    const std::string str = (const std::string)((*I).first);
    renderer->print(CONSOLE_TEXT_OFFSET_X, height - ((i + 1) * FONT_HEIGHT ), const_cast<char*>(str.c_str()), 0);
  }
  //Remove expired messages
  //TODO this currently only removes the messges from  the top of the list
  //     should a messages at the top have a longer expiry time than one
  //     later in the list, the later one will stay in the list until the 
  //     higher entries have been removed
  bool loop = true;
  while (loop) {
  if (screen_messages.empty()) break;	
    loop = false; // break unless it gets set to true again
    screenMessage sm = *screen_messages.begin(); // Get first messge
    unsigned int message_time = sm.second;
    if (current_time > message_time) { // Check expiry time
      screen_messages.erase(screen_messages.begin());
      loop = true; // Go again
    }
  }
}

void Console::toggleConsole() {
  assert ((_initialised == true) && "Console not initialised");
  // Start the animation	
  animateConsole = 1;
  // Toggle state
  showConsole = !showConsole;
}

void Console::registerCommand(const std::string &command, ConsoleObject *object) {
  if (debug) Log::writeLog(std::string("registering: ") + command, Log::LOG_INFO);
  // Assign the ConsoleObject to the command
  _registered_commands[command] = object;
}

void Console::runCommand(const std::string &comd) {
  assert ((_initialised == true) && "Console not initialised");
  if (comd.empty()) return; // Ignore empty string
  std::string command = comd;
  System::instance()->getFileHandler()->expandString(command);
  // Grab first character of command string
  char c = command.c_str()[0];
  // Check to see if command is a command, or a speech string
  if ((c != '/' && c != '+' && c != '-')) {
    // Its a speech string, so SAY it
    // FIXME /say is not always available!
    if (_registered_commands[SAY]) {
      runCommand(std::string(CMD_SAY) + command);
    } else {
      if (debug) Log::writeLog(std::string("Cannot SAY, not in game yet: ") + command, Log::LOG_ERROR);
      pushMessage("Cannot SAY, not it game yet" , CONSOLE_MESSAGE, 0);
    }
    return; 
  }
  // If command has a leading /, remove it
  std::string command_string = (c == '/')? command.substr(1) : command;
  // Split string into command / arguments pair
  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(command_string);
  std::string cmd = tokeniser.nextToken();
  std::string args = tokeniser.remainingTokens();
  //Grab object registered to the command
  ConsoleObject* con_obj = _registered_commands[cmd];
  // Print all commands apart form toggle console to the console
  if (cmd != TOGGLE_CONSOLE) pushMessage(command_string, CONSOLE_MESSAGE, 0);
  // If object exists, run the command
  if (con_obj) con_obj->runCommand(cmd, args);
  else { // Else print error message
    if (debug) Log::writeLog(std::string("Unknown command: ") + command, Log::LOG_ERROR);
    pushMessage("Unknown command" , CONSOLE_MESSAGE, 0);
  }
}

void Console::runCommand(const std::string &command, const std::string &args) {
  assert ((_initialised == true) && "Console not initialised");
  // This command toggles the console
  if (command == TOGGLE_CONSOLE) {
    toggleConsole();
  }
  // This commands prints all currently registers commands to the Log File
  else if (command == LIST_CONSOLE_COMMANDS) {
    for (std::map<std::string, ConsoleObject*>::const_iterator I = _registered_commands.begin(); I != _registered_commands.end(); ++I) {
      // TODO - should we check to see if I->second is valid?
//      if (debug)
	      Log::writeLog(I->first, Log::LOG_INFO);
    }
  }
}

} /* namespace Sear */
