// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton


#include <SDL/SDL.h>

#include "common/Config.h"
#include "common/Log.h"

#include "Bindings.h"

namespace Sear {

// Static Declarations	
std::map<int, std::string> Bindings::keymap = std::map<int, std::string>();
Config *Bindings::_bindings = NULL;

void Bindings::init() {
  _bindings = new Config(); // Create a new config object to store data in
  initKeyMap(); // Initilise key mappings
}

void Bindings::initKeyMap() {
  keymap = std::map<int, std::string>(); // Create an empty mapping
  // Assign keys to textual representation
  keymap[SDLK_BACKSPACE] = "{BACKSPACE}";
  keymap[SDLK_TAB] = "{TAB}";
  keymap[SDLK_CLEAR] = "{CLEAR}";
  keymap[SDLK_RETURN] = "{RETURN}";
  keymap[SDLK_PAUSE] = "{PAUSE}";
  keymap[SDLK_ESCAPE] = "{ESCAPE}";
  keymap[SDLK_SPACE] = "{SPACE}";
  keymap[SDLK_EXCLAIM] = "!";
  keymap[SDLK_QUOTEDBL] = "{DBL_QUOTE}";
  keymap[SDLK_HASH] = "#";
  keymap[SDLK_DOLLAR] = "$";
  keymap[SDLK_AMPERSAND] = "&";
  keymap[SDLK_QUOTE] = "{QUOTE}";
  keymap[SDLK_LEFTPAREN] = "(";
  keymap[SDLK_RIGHTPAREN] = "(";
  keymap[SDLK_ASTERISK] = "*";
  keymap[SDLK_PLUS] = "+";
  keymap[SDLK_COMMA] = ",";
  keymap[SDLK_MINUS] = "-";
  keymap[SDLK_PERIOD] = ".";
  keymap[SDLK_SLASH] = "/";
  keymap[SDLK_0] = "0";
  keymap[SDLK_1] = "1";
  keymap[SDLK_2] = "2";
  keymap[SDLK_3] = "3";
  keymap[SDLK_4] = "4";
  keymap[SDLK_5] = "5";
  keymap[SDLK_6] = "6";
  keymap[SDLK_7] = "7";
  keymap[SDLK_8] = "8";
  keymap[SDLK_9] = "9";
  keymap[SDLK_COLON] = ":";
  keymap[SDLK_SEMICOLON] = ";";
  keymap[SDLK_LESS] = "<";
  keymap[SDLK_EQUALS] = "{EQUALS}";
  keymap[SDLK_GREATER] = ">";
  keymap[SDLK_QUESTION] = "?";
  keymap[SDLK_AT] = "@";
  keymap[SDLK_LEFTBRACKET] = "{LEFT_BRACE}";
  keymap[SDLK_BACKSLASH] = "\\";
  keymap[SDLK_RIGHTBRACKET] = "{RIGHT_BRACE}";
  keymap[SDLK_CARET] = "|";
  keymap[SDLK_UNDERSCORE] = "_";
  keymap[SDLK_BACKQUOTE] = "{BACKQUOTE}";
  keymap[SDLK_a] = "a";
  keymap[SDLK_b] = "b";
  keymap[SDLK_c] = "c";
  keymap[SDLK_d] = "d";
  keymap[SDLK_e] = "e";
  keymap[SDLK_f] = "f";
  keymap[SDLK_g] = "g";
  keymap[SDLK_h] = "h";
  keymap[SDLK_i] = "i";
  keymap[SDLK_j] = "j";
  keymap[SDLK_k] = "k";
  keymap[SDLK_l] = "l";
  keymap[SDLK_m] = "m";
  keymap[SDLK_n] = "n";
  keymap[SDLK_o] = "o";
  keymap[SDLK_p] = "p";
  keymap[SDLK_q] = "q";
  keymap[SDLK_r] = "r";
  keymap[SDLK_s] = "s";
  keymap[SDLK_t] = "t";
  keymap[SDLK_u] = "u";
  keymap[SDLK_v] = "v";
  keymap[SDLK_w] = "w";
  keymap[SDLK_x] = "x";
  keymap[SDLK_y] = "y";
  keymap[SDLK_z] = "z";
  keymap[SDLK_DELETE] = "{DELETE}";
  keymap[SDLK_KP0] = "{KP_0}";
  keymap[SDLK_KP1] = "{KP_1}";
  keymap[SDLK_KP2] = "{KP_2}";
  keymap[SDLK_KP3] = "{KP_3}";
  keymap[SDLK_KP4] = "{KP_4}";
  keymap[SDLK_KP5] = "{KP_5}";
  keymap[SDLK_KP6] = "{KP_6}";
  keymap[SDLK_KP7] = "{KP_7}";
  keymap[SDLK_KP8] = "{KP_8}";
  keymap[SDLK_KP9] = "{KP_9}";
  keymap[SDLK_KP_PERIOD] = "{KP_PERIOD}";
  keymap[SDLK_KP_DIVIDE] = "{KP_DIVIDE}";
  keymap[SDLK_KP_MULTIPLY] = "{KP_MULTI}";
  keymap[SDLK_KP_MINUS] = "KP_MINUS}";
  keymap[SDLK_KP_PLUS] = "{KP_PLUS}";
  keymap[SDLK_KP_ENTER] = "{KP_ENTER}";
  keymap[SDLK_KP_EQUALS] = "{KP_EQUALS}";
  keymap[SDLK_UP] = "{UP}";
  keymap[SDLK_DOWN] = "{DOWN}";
  keymap[SDLK_RIGHT] = "{RIGHT}";
  keymap[SDLK_LEFT] = "{LEFT}";
  keymap[SDLK_INSERT] = "{INSERT}";
  keymap[SDLK_HOME] = "{HOME}";
  keymap[SDLK_END] = "{END}";
  keymap[SDLK_PAGEUP] = "{PAGE_UP}";
  keymap[SDLK_PAGEDOWN] = "{PAGE_DOWN}";
  keymap[SDLK_F1] = "{F1}";
  keymap[SDLK_F2] = "{F2}";
  keymap[SDLK_F3] = "{F3}";
  keymap[SDLK_F4] = "{F4}";
  keymap[SDLK_F5] = "{F5}";
  keymap[SDLK_F6] = "{F6}";
  keymap[SDLK_F7] = "{F7}";
  keymap[SDLK_F8] = "{F8}";
  keymap[SDLK_F9] = "{F9}";
  keymap[SDLK_F10] = "{F10}";
  keymap[SDLK_F11] = "{F11}";
  keymap[SDLK_F12] = "{F12}";
  keymap[SDLK_F13] = "{F13}";
  keymap[SDLK_F14] = "{F14}";
  keymap[SDLK_F15] = "{F15}";

  keymap[SDLK_NUMLOCK] = "{NUM}";
  keymap[SDLK_CAPSLOCK] = "{CAPS}";
  keymap[SDLK_SCROLLOCK] = "{SRCOLL}";
  keymap[SDLK_RSHIFT] = "{RIGHT_SHIFT}";
  keymap[SDLK_LSHIFT] = "{LEFT_SHIFT}";
  keymap[SDLK_RCTRL] = "{RIGHT_CRTL}";
  keymap[SDLK_LCTRL] = "{LEFT_CTRL}";
  keymap[SDLK_RALT] = "{RIGHT_ALT}";
  keymap[SDLK_LALT] = "{LEFT_ALT}";
  keymap[SDLK_RMETA] = "{RIGHT_META}";
  keymap[SDLK_LMETA] = "{LEFT_META}";
  keymap[SDLK_LSUPER] = "{LEFT_SUPER}";
  keymap[SDLK_RSUPER] = "{RIGHT_SUPER}";
  keymap[SDLK_MODE]= "{MODE}";
  keymap[SDLK_COMPOSE] = "{COMPOSE}";
  keymap[SDLK_PRINT] = "{PRINT}";
  keymap[SDLK_SYSREQ] = "{SYSREQ}";
  keymap[SDLK_BREAK] = "{BREAK}";
  keymap[SDLK_MENU] = "{MENU}";
  keymap[SDLK_POWER] = "{POWER}";
  keymap[SDLK_EURO] = "{EURO}";
}

void Bindings::shutdown() {
  if (_bindings) {
    _bindings->shutdown();
    delete _bindings;
    _bindings = NULL;
  }
  // Delete keymap - is this necessary?
  while (!keymap.empty()) {
    keymap.erase(keymap.begin());
  }
}

void Bindings::loadBindings(const std::string &file_name) {
  // Merges key bindings file, file_name with existing contents
  if (_bindings) _bindings->loadConfig(file_name);
  else Log::writeLog("Bindings: Error - bindings config object not created", Log::LOG_ERROR);
}

void Bindings::saveBindings() {
  // Save current key bindings to last used file name
  if (_bindings) _bindings->saveConfig();
  else Log::writeLog("Bindings: Error - bindings config object not created", Log::LOG_ERROR);
}
void Bindings::saveBindings(const std::string &file_name) {
  // Save current key bindings to file_name
  if (_bindings) _bindings->saveConfig(file_name);
  else Log::writeLog("Bindings: Error - bindings config object not created", Log::LOG_ERROR);
}

void Bindings::bind(std::string key, std::string command) {
  if (key.empty()) return; // Check we were sent a key
//  if (command.empty()) command = "{UNBOUND}"; // If command is empty we are unbinding the key. Need some text to avoid empty string
  if (_bindings) _bindings->setAttribute(key, command); // Store new binding
  else Log::writeLog("Bindings: Error - bindings config object not created", Log::LOG_ERROR);
}

std::string Bindings::idToString(int key) {
  // Return the key mapping.
  return keymap[key];
}

std::string Bindings::getBinding(const std::string &key) {
  // Check if bindings object has been created
  if (!_bindings) {
    Log::writeLog("Bindings: Error - bindings config object not created", Log::LOG_ERROR);
    return "";
  }
  if (key.empty()) return "";
  // Retrive current binding
  std::string cmd = _bindings->getAttribute(key);
  if (cmd.empty()) { // Retrieved command should not be the empty string
//    _bindings->setAttribute(key, "{UNBOUND}");
    return "";
  }
  return cmd; // Return retrieved binding
}


}
