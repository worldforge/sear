// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: Bindings.cpp,v 1.19 2005-05-16 19:40:32 simon Exp $


#include <SDL/SDL.h>

#include <varconf/Config.h>

#include "common/Log.h"

#include "Bindings.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

// Static Declarations	
std::map<int, std::string> Bindings::m_keymap = std::map<int, std::string>();
varconf::Config *Bindings::m_bindings = NULL;

void Bindings::init() {
  m_bindings = new varconf::Config(); // Create a new config object to store data in
  initKeyMap(); // Initilise key mappings
}

void Bindings::initKeyMap() {
  assert((m_bindings != NULL) && "Bindings config is NULL");
  m_keymap = std::map<int, std::string>(); // Create an empty mapping
  // Assign keys to textual representation
  m_keymap[SDLK_BACKSPACE] = "backspace";
  m_keymap[SDLK_TAB] = "tab";
  m_keymap[SDLK_CLEAR] = "clear";
  m_keymap[SDLK_RETURN] = "return";
  m_keymap[SDLK_PAUSE] = "pause";
  m_keymap[SDLK_ESCAPE] = "escape";
  m_keymap[SDLK_SPACE] = "space";
  m_keymap[SDLK_EXCLAIM] = "exclaim";
  m_keymap[SDLK_QUOTEDBL] = "dbl_quote";
  m_keymap[SDLK_HASH] = "hash";
  m_keymap[SDLK_DOLLAR] = "dollar";
  m_keymap[SDLK_AMPERSAND] = "ampersand";
  m_keymap[SDLK_QUOTE] = "quote";
  m_keymap[SDLK_LEFTPAREN] = "left_paren";
  m_keymap[SDLK_RIGHTPAREN] = "right_paren";
  m_keymap[SDLK_ASTERISK] = "asterisk";
  m_keymap[SDLK_PLUS] = "plus";
  m_keymap[SDLK_COMMA] = "comma";
  m_keymap[SDLK_MINUS] = "-";
  m_keymap[SDLK_PERIOD] = "period";
  m_keymap[SDLK_SLASH] = "slash";
  m_keymap[SDLK_0] = "0";
  m_keymap[SDLK_1] = "1";
  m_keymap[SDLK_2] = "2";
  m_keymap[SDLK_3] = "3";
  m_keymap[SDLK_4] = "4";
  m_keymap[SDLK_5] = "5";
  m_keymap[SDLK_6] = "6";
  m_keymap[SDLK_7] = "7";
  m_keymap[SDLK_8] = "8";
  m_keymap[SDLK_9] = "9";
  m_keymap[SDLK_COLON] = "colon";
  m_keymap[SDLK_SEMICOLON] = "semi_colon";
  m_keymap[SDLK_LESS] = "less_than";
  m_keymap[SDLK_EQUALS] = "equals";
  m_keymap[SDLK_GREATER] = "greater_then";
  m_keymap[SDLK_QUESTION] = "question";
  m_keymap[SDLK_AT] = "at";
  m_keymap[SDLK_LEFTBRACKET] = "left_brace";
  m_keymap[SDLK_BACKSLASH] = "backslash";
  m_keymap[SDLK_RIGHTBRACKET] = "right_brace";
  m_keymap[SDLK_CARET] = "caret";
  m_keymap[SDLK_UNDERSCORE] = "_";
  m_keymap[SDLK_BACKQUOTE] = "backquote";
  m_keymap[SDLK_a] = "a";
  m_keymap[SDLK_b] = "b";
  m_keymap[SDLK_c] = "c";
  m_keymap[SDLK_d] = "d";
  m_keymap[SDLK_e] = "e";
  m_keymap[SDLK_f] = "f";
  m_keymap[SDLK_g] = "g";
  m_keymap[SDLK_h] = "h";
  m_keymap[SDLK_i] = "i";
  m_keymap[SDLK_j] = "j";
  m_keymap[SDLK_k] = "k";
  m_keymap[SDLK_l] = "l";
  m_keymap[SDLK_m] = "m";
  m_keymap[SDLK_n] = "n";
  m_keymap[SDLK_o] = "o";
  m_keymap[SDLK_p] = "p";
  m_keymap[SDLK_q] = "q";
  m_keymap[SDLK_r] = "r";
  m_keymap[SDLK_s] = "s";
  m_keymap[SDLK_t] = "t";
  m_keymap[SDLK_u] = "u";
  m_keymap[SDLK_v] = "v";
  m_keymap[SDLK_w] = "w";
  m_keymap[SDLK_x] = "x";
  m_keymap[SDLK_y] = "y";
  m_keymap[SDLK_z] = "z";
  m_keymap[SDLK_DELETE] = "delete";
  m_keymap[SDLK_KP0] = "kp_0";
  m_keymap[SDLK_KP1] = "kp_1";
  m_keymap[SDLK_KP2] = "kp_2";
  m_keymap[SDLK_KP3] = "kp_3";
  m_keymap[SDLK_KP4] = "kp_4";
  m_keymap[SDLK_KP5] = "kp_5";
  m_keymap[SDLK_KP6] = "kp_6";
  m_keymap[SDLK_KP7] = "kp_7";
  m_keymap[SDLK_KP8] = "kp_8";
  m_keymap[SDLK_KP9] = "kp_9";
  m_keymap[SDLK_KP_PERIOD] = "kp_period";
  m_keymap[SDLK_KP_DIVIDE] = "kp_divide";
  m_keymap[SDLK_KP_MULTIPLY] = "kp_multi";
  m_keymap[SDLK_KP_MINUS] = "kp_minus";
  m_keymap[SDLK_KP_PLUS] = "kp_plus";
  m_keymap[SDLK_KP_ENTER] = "kp_enter";
  m_keymap[SDLK_KP_EQUALS] = "kp_equals";
  m_keymap[SDLK_UP] = "up";
  m_keymap[SDLK_DOWN] = "down";
  m_keymap[SDLK_RIGHT] = "right";
  m_keymap[SDLK_LEFT] = "left";
  m_keymap[SDLK_INSERT] = "insert";
  m_keymap[SDLK_HOME] = "home";
  m_keymap[SDLK_END] = "end";
  m_keymap[SDLK_PAGEUP] = "page_up";
  m_keymap[SDLK_PAGEDOWN] = "page_down";
  m_keymap[SDLK_F1] = "f1";
  m_keymap[SDLK_F2] = "f2";
  m_keymap[SDLK_F3] = "f3";
  m_keymap[SDLK_F4] = "f4";
  m_keymap[SDLK_F5] = "f5";
  m_keymap[SDLK_F6] = "f6";
  m_keymap[SDLK_F7] = "f7";
  m_keymap[SDLK_F8] = "f8";
  m_keymap[SDLK_F9] = "f9";
  m_keymap[SDLK_F10] = "f10";
  m_keymap[SDLK_F11] = "f11";
  m_keymap[SDLK_F12] = "f12";
  m_keymap[SDLK_F13] = "f13";
  m_keymap[SDLK_F14] = "f14";
  m_keymap[SDLK_F15] = "f15";

  m_keymap[SDLK_NUMLOCK] = "num";
  m_keymap[SDLK_CAPSLOCK] = "caps";
  m_keymap[SDLK_SCROLLOCK] = "srcoll";
  m_keymap[SDLK_RSHIFT] = "right_shift";
  m_keymap[SDLK_LSHIFT] = "left_shift";
  m_keymap[SDLK_RCTRL] = "right_ctrl";
  m_keymap[SDLK_LCTRL] = "left_ctrl";
  m_keymap[SDLK_RALT] = "right_alt";
  m_keymap[SDLK_LALT] = "left_alt";
  m_keymap[SDLK_RMETA] = "right_meta";
  m_keymap[SDLK_LMETA] = "left_meta";
  m_keymap[SDLK_LSUPER] = "left_super";
  m_keymap[SDLK_RSUPER] = "right_super";
  m_keymap[SDLK_MODE]= "mode";
  m_keymap[SDLK_COMPOSE] = "compose";
  m_keymap[SDLK_PRINT] = "print";
  m_keymap[SDLK_SYSREQ] = "sysreq";
  m_keymap[SDLK_BREAK] = "break";
  m_keymap[SDLK_MENU] = "menu";
  m_keymap[SDLK_POWER] = "power";
  m_keymap[SDLK_EURO] = "euro";
}

void Bindings::shutdown() {
  if (m_bindings) {
    delete m_bindings;
    m_bindings = NULL;
  }
  // Delete m_keymap
  while (!m_keymap.empty()) {
    m_keymap.erase(m_keymap.begin());
  }
}

void Bindings::loadBindings(const std::string &file_name) {
  assert((m_bindings != NULL) && "Bindings config is NULL");
  // Merges key bindings file, file_name with existing contents
  if (!m_bindings->readFromFile(file_name)) 
    Log::writeLog(std::string("Error processing ") + file_name, Log::LOG_ERROR);
}

void Bindings::saveBindings(const std::string &file_name) {
  assert((m_bindings != NULL) && "Bindings config is NULL");
  // Save current key bindings to file_name
  m_bindings->writeToFile(file_name);
}

void Bindings::bind(std::string key, std::string command) {
  assert((m_bindings != NULL) && "Bindings config is NULL");
  if (key.empty()) return; // Check we were sent a key
  m_bindings->setItem("key_bindings", key, command); // Store new binding
}

std::string Bindings::idToString(int key) {
  assert((m_bindings != NULL) && "Bindings config is NULL");
  // Return the key mapping.
  return m_keymap[key];
}

std::string Bindings::getBinding(const std::string &key) {
  // Check if bindings object has been created
  assert((m_bindings != NULL) && "Bindings config is NULL");
  if (key.empty()) return "";
  // Retrive current binding
  std::string the_key = std::string(key);
  m_bindings->clean(the_key);
  std::string cmd = m_bindings->getItem("key_bindings", the_key);
  if (cmd.empty()) { // Retrieved command should not be the empty string
    return "";
  }
  return cmd; // Return retrieved binding
}

std::string Bindings::getBindingForKeysym(const SDL_keysym& key) {
  assert(m_bindings);
  
  if (!m_keymap.count(key.sym)) return ""; // un-mapped basic keysym
  std::string plainName = m_keymap[key.sym],
    decoratedName = plainName;
    
  if (key.mod & KMOD_SHIFT)
    decoratedName = "shift_" + decoratedName;    
    
  if (key.mod & KMOD_ALT)
    decoratedName = "alt_" + decoratedName;
    
  if (key.mod & KMOD_CTRL)
    decoratedName = "ctrl_" + decoratedName;
    
  m_bindings->clean(decoratedName);
  if (m_bindings->findItem("key_bindings", decoratedName))
    return m_bindings->getItem("key_bindings", decoratedName);
    
  if (m_bindings->findItem("key_bindings", plainName))
    return m_bindings->getItem("key_bindings", plainName);
    
  std::cout << "no binding specified for key " << decoratedName << std::endl;
  return "";
}
  
}
