// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "common/Utility.h"
#include "common/Log.h"

#include "Console.h"
#include "ConsoleObject.h"
#include "System.h"
#include "Graphics.h"
#include "Render.h"

namespace Sear {

Console::Console(System *system) :
  animateConsole(0),
  showConsole(0),
  consoleHeight(0),
  console_messages(std::list<std::string>()),
  screen_messages(std::list<screenMessage>()),
  _system(system),
  _renderer(NULL)
{ }

Console::~Console() {}

bool Console::init() {
  _renderer = _system->getGraphics()->getRender();
  panel_id = _renderer->requestTexture("ui", "panel");
  registerCommand(TOGGLE_CONSOLE, this);
  registerCommand(LIST_CONSOLE_COMMANDS, this);
  return true;
}

void Console::shutdown() {
  Log::writeLog("Shutting down console.", Log::LOG_DEFAULT);
}

//void Console::PushMessage(const char *message, char* extra) {
void Console::pushMessage(const std::string &message, int type, int duration) {
  if (type & SCREEN_MESSAGE) {	
    if (screen_messages.size() >= MAX_MESSAGES) screen_messages.erase(screen_messages.begin());
    screen_messages.push_back(screenMessage(message, _system->getTime() + duration));
  }
  if (type & CONSOLE_MESSAGE) {
    if (console_messages.size() >= MAX_MESSAGES) console_messages.erase(console_messages.begin());
    console_messages.push_back(message);
  }
}

void Console::draw(const std::string &command) {
  if (animateConsole && showConsole) {
    consoleHeight += CONSOLE_SPEED;
    if (consoleHeight >= CONSOLE_HEIGHT) {
      consoleHeight = CONSOLE_HEIGHT;
      animateConsole = 0;
    }
    renderConsoleMessages(command);
  } else if (animateConsole && !showConsole) {
    consoleHeight -= CONSOLE_SPEED;
    if (consoleHeight <= 0) {
      consoleHeight = 0;
      animateConsole = 0;
    }
    renderConsoleMessages(command);
  } else if (showConsole) {
    renderConsoleMessages(command);
  }
  renderScreenMessages();
}

void Console::renderConsoleMessages(const std::string &command) {
  if (!_renderer) {
    Log::writeLog("Console: Error - Renderer object not created", Log::LOG_ERROR);
    return;
  }
  std::list<std::string>::const_iterator I;
  int i;
  int consoleOffset = CONSOLE_HEIGHT - consoleHeight;
//  _renderer->stateChange(Render::PANEL);
//  _renderer->nextState(FONT_TO_PANEL);
  _renderer->stateChange("panel");
  _renderer->setColour(0.0f, 0.0f, 1.0f, 0.85f);
  _renderer->drawTextRect(0, 0, _renderer->getWindowWidth(), consoleHeight, panel_id);
//  _renderer->stateChange(Render::CONSOLE);
//  _renderer->nextState(PANEL_TO_FONT);
  _renderer->stateChange("font");
  _renderer->setColour(1.0f, 1.0f, 0.0f, 1.0f);
  for (I = console_messages.begin(), i = 0; I != console_messages.end(); I++, i++) {
    int j = console_messages.size() - i;
    _renderer->print(CONSOLE_TEXT_OFFSET_X, CONSOLE_TEXT_OFFSET_Y + j * FONT_HEIGHT - consoleOffset, (char*)(*I).c_str(), 0);
  }
  std::string str = CONSOLE_PROMPT_STRING + command + CONSOLE_CURSOR_STRING;
  _renderer->print(CONSOLE_TEXT_OFFSET_X, CONSOLE_TEXT_OFFSET_Y - consoleOffset, (char *)str.c_str(), 0);
}

void Console::renderScreenMessages() {
  if (screen_messages.empty()) return;	
  std::list<screenMessage>::iterator I;
  int i;
  _renderer->stateChange("font");
  _renderer->setColour(1.0f, 1.0f, 0.0f, 0.85f);
  int height = _renderer->getWindowHeight();
  unsigned int current_time = _system->getTime();
  
  for (I = screen_messages.begin(), i = 0; I != screen_messages.end(); I++, i++) {
    const std::string str = (const std::string)((*I).first);
    _renderer->print(CONSOLE_TEXT_OFFSET_X, height - ((i + 1) * FONT_HEIGHT ), (char*)str.c_str(), 0);
  }
  bool loop = true;
  while (loop) {
  if (screen_messages.empty()) break;	
    loop = false;
    screenMessage sm = *screen_messages.begin();
    unsigned int message_time = sm.second;
    if (current_time > message_time) {
      screen_messages.erase(screen_messages.begin());
      loop = true;
    }
  }
}

void Console::toggleConsole() {
  animateConsole = 1;
  showConsole = !showConsole;
}

void Console::registerCommand(const std::string &command, ConsoleObject *object) {
  Log::writeLog(std::string("registering: ") + command, Log::LOG_INFO);
  _registered_commands[command] = object;
}

void Console::runCommand(const std::string &command_string) {
  if (command_string.empty()) return; // Ignore no commanc
  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(command_string);
  std::string command = tokeniser.nextToken();
  std::string args = tokeniser.remainingTokens();
  ConsoleObject* con_obj = _registered_commands[command];
  if (command != TOGGLE_CONSOLE) pushMessage(command_string, CONSOLE_MESSAGE, 0);
  if (con_obj) con_obj->runCommand(command, args);
  else {
    Log::writeLog(std::string("Unknown command: ") + command, Log::LOG_ERROR);
    pushMessage("Unknown command" , CONSOLE_MESSAGE, 0);
  }
}

void Console::runCommand(const std::string &command, const std::string &args) {
  if (command == TOGGLE_CONSOLE) {
    toggleConsole();
  }
  else if (command == LIST_CONSOLE_COMMANDS) {
    for (std::map<std::string, ConsoleObject*>::const_iterator I = _registered_commands.begin(); I != _registered_commands.end(); I++) {
      Log::writeLog(I->first, Log::LOG_INFO);
    }
  }
}

} /* namespace Sear */
