// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: Bindings.cpp,v 1.10 2002-09-08 00:24:53 simon Exp $

#include <SDL/SDL.h>

#include <varconf/Config.h>

#include "common/Log.h"

#include "Bindings.h"

namespace Sear {

// Static Declarations	
std::map<int, std::string> Bindings::keymap = std::map<int, std::string>();
varconf::Config *Bindings::_bindings = NULL;

void Bindings::init() {
  _bindings = new varconf::Config(); // Create a new config object to store data in
  initKeyMap(); // Initilise key mappings
}

void Bindings::initKeyMap() {
  keymap = std::map<int, std::string>(); // Create an empty mapping
  // Assign keys to textual representation
  keymap[SDLK_BACKSPACE] = "backspace";
  keymap[SDLK_TAB] = "tab";
  keymap[SDLK_CLEAR] = "clear";
  keymap[SDLK_RETURN] = "return";
  keymap[SDLK_PAUSE] = "pause";
  keymap[SDLK_ESCAPE] = "escape";
  keymap[SDLK_SPACE] = "space";
  keymap[SDLK_EXCLAIM] = "exclaim";
  keymap[SDLK_QUOTEDBL] = "dbl_quote";
  keymap[SDLK_HASH] = "hash";
  keymap[SDLK_DOLLAR] = "dollar";
  keymap[SDLK_AMPERSAND] = "ampersand";
  keymap[SDLK_QUOTE] = "quote";
  keymap[SDLK_LEFTPAREN] = "left_paren";
  keymap[SDLK_RIGHTPAREN] = "right_paren";
  keymap[SDLK_ASTERISK] = "asterisk";
  keymap[SDLK_PLUS] = "plus";
  keymap[SDLK_COMMA] = "comma";
  keymap[SDLK_MINUS] = "-";
  keymap[SDLK_PERIOD] = "period";
  keymap[SDLK_SLASH] = "slash";
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
  keymap[SDLK_COLON] = "colon";
  keymap[SDLK_SEMICOLON] = "semi_colon";
  keymap[SDLK_LESS] = "less_than";
  keymap[SDLK_EQUALS] = "equals";
  keymap[SDLK_GREATER] = "greater_then";
  keymap[SDLK_QUESTION] = "question";
  keymap[SDLK_AT] = "at";
  keymap[SDLK_LEFTBRACKET] = "left_brace";
  keymap[SDLK_BACKSLASH] = "backslash";
  keymap[SDLK_RIGHTBRACKET] = "right_brace";
  keymap[SDLK_CARET] = "caret";
  keymap[SDLK_UNDERSCORE] = "_";
  keymap[SDLK_BACKQUOTE] = "backquote";
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
  keymap[SDLK_DELETE] = "delete";
  keymap[SDLK_KP0] = "kp_0";
  keymap[SDLK_KP1] = "kp_1";
  keymap[SDLK_KP2] = "kp_2";
  keymap[SDLK_KP3] = "kp_3";
  keymap[SDLK_KP4] = "kp_4";
  keymap[SDLK_KP5] = "kp_5";
  keymap[SDLK_KP6] = "kp_6";
  keymap[SDLK_KP7] = "kp_7";
  keymap[SDLK_KP8] = "kp_8";
  keymap[SDLK_KP9] = "kp_9";
  keymap[SDLK_KP_PERIOD] = "kp_period";
  keymap[SDLK_KP_DIVIDE] = "kp_divide";
  keymap[SDLK_KP_MULTIPLY] = "kp_multi";
  keymap[SDLK_KP_MINUS] = "kp_minus";
  keymap[SDLK_KP_PLUS] = "kp_plus";
  keymap[SDLK_KP_ENTER] = "kp_enter";
  keymap[SDLK_KP_EQUALS] = "kp_equals";
  keymap[SDLK_UP] = "up";
  keymap[SDLK_DOWN] = "down";
  keymap[SDLK_RIGHT] = "right";
  keymap[SDLK_LEFT] = "left";
  keymap[SDLK_INSERT] = "insert";
  keymap[SDLK_HOME] = "home";
  keymap[SDLK_END] = "end";
  keymap[SDLK_PAGEUP] = "page_up";
  keymap[SDLK_PAGEDOWN] = "page_down";
  keymap[SDLK_F1] = "f1";
  keymap[SDLK_F2] = "f2";
  keymap[SDLK_F3] = "f3";
  keymap[SDLK_F4] = "f4";
  keymap[SDLK_F5] = "f5";
  keymap[SDLK_F6] = "f6";
  keymap[SDLK_F7] = "f7";
  keymap[SDLK_F8] = "f8";
  keymap[SDLK_F9] = "f9";
  keymap[SDLK_F10] = "f10";
  keymap[SDLK_F11] = "f11";
  keymap[SDLK_F12] = "f12";
  keymap[SDLK_F13] = "f13";
  keymap[SDLK_F14] = "f14";
  keymap[SDLK_F15] = "f15";

  keymap[SDLK_NUMLOCK] = "num";
  keymap[SDLK_CAPSLOCK] = "caps";
  keymap[SDLK_SCROLLOCK] = "srcoll";
  keymap[SDLK_RSHIFT] = "right_shift";
  keymap[SDLK_LSHIFT] = "left_shift";
  keymap[SDLK_RCTRL] = "right_crtl";
  keymap[SDLK_LCTRL] = "left_ctrl";
  keymap[SDLK_RALT] = "right_alt";
  keymap[SDLK_LALT] = "left_alt";
  keymap[SDLK_RMETA] = "right_meta";
  keymap[SDLK_LMETA] = "left_meta";
  keymap[SDLK_LSUPER] = "left_super";
  keymap[SDLK_RSUPER] = "right_super";
  keymap[SDLK_MODE]= "mode";
  keymap[SDLK_COMPOSE] = "compose";
  keymap[SDLK_PRINT] = "print";
  keymap[SDLK_SYSREQ] = "sysreq";
  keymap[SDLK_BREAK] = "break";
  keymap[SDLK_MENU] = "menu";
  keymap[SDLK_POWER] = "power";
  keymap[SDLK_EURO] = "euro";
}

void Bindings::shutdown() {
  if (_bindings) {
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
  if (_bindings){ 
    if (!_bindings->readFromFile(file_name)) 
      Log::writeLog(std::string("Error processing ") + file_name, Log::LOG_ERROR);
  }
  else Log::writeLog("Bindings: Error - bindings config object not created", Log::LOG_ERROR);
}

void Bindings::saveBindings(const std::string &file_name) {
  // Save current key bindings to file_name
  if (_bindings) _bindings->writeToFile(file_name);
  else Log::writeLog("Bindings: Error - bindings config object not created", Log::LOG_ERROR);
}

void Bindings::bind(std::string key, std::string command) {
  if (key.empty()) return; // Check we were sent a key
  if (_bindings) _bindings->setItem("key_bindings", key, command); // Store new binding
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
  std::string the_key = std::string(key);
  _bindings->clean(the_key);
  std::string cmd = _bindings->getItem("key_bindings", the_key);
  if (cmd.empty()) { // Retrieved command should not be the empty string
//    _bindings->setAttribute(key, "{UNBOUND}");
    return "";
  }
  return cmd; // Return retrieved binding
}


}
