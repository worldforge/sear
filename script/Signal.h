// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: Signal.h,v 1.1.2.1 2003-01-23 20:18:25 simon Exp $

#ifndef SEAR_SCRIPTENGINE_SIGNAL_H
#define SEAR_SCRIPTENGINE_SIGNAL_H 1

#include "lua_value.h"

class SignalHandler {
public:
  typedef std::list<lua_value> ArgList;
  typedef lua_State* ScriptRef;
	
  Signal();
  ~Signal();

  void init();
  void shutdown();

  void registerCallback(const std::string &callback) {
    // Create a new slot list for the callback
    if (!_signal_map[callback]) _signal_map[callback] = new SlotList();
    else ; //ERROR
  }
  void unregisterCallback(const std::string &callback) {
    // remove slot list for the callback
    if (_signal_map[callback]) delete _signal_map[callback];
    else ; //ERROR
    _signal_map[callback] = NULL;
  }

  void fireCallback(const std::string &callback, ArgList &args) {
    // Get slot list if it exists
    SlotList *list = _signal_map[callback];
    if (!list) return;
    if (list->empty()) return;
    // loop through all connected signals
    for (SlotList::const_iterator I = list->begin(); I != list->end(); ++I) {
      SlotItem &si = *I;
      // Push function onto stack
      lua_getglobal(si.first, si.second.c_str());
      // push args onto stack
      int arg_counter = 0;
      for (ArgList::const_iterator J = args.begin(); J != args.endi(); ++J) {
	lua_value &v = *J;
        if (v.isNil()) lua_pushNil(L);
	else if (v.isNumber()) lua_pushnumber(L, v.asNumber());
	else if (v.isString()) lua_pushstring(L, v.asString());
	else if (v.isTable()) lua_pushtable(L, v.asTable());
	else if (v.isUserdata()) lua_pushusertag(L, v.asUserdata());
	else if (v.isFunction()) lua_pushcfunction(L, v.asFunction());
	++arg_counter;
      }
      // CAll function
      lua_call(L, arg_counter, 0);
    }
  }

  void registerCommands(ScriptRef L) {
    lua_register(L, "connect", &SignalHandler::l_connect);
    lua_register(L, "disconnect", &SignalHandler::l_disconnect);
  }
  
private:
  typedef std::pair<ScriptRef, std::string> SlotItem; // Script state / function name
  typedef std::list<SlotItem> SlotList;
  typedef std::map<std::string, *SlotList> SignalMap;

  SignalMap _signal_map;
	
  static int l_connect(ScriptRef L) {
    // Check arg number and types
    std::string callback = lua_tostring(L, 1);
    std::string function = lua_tostring(L, 2);
    //Check callback list exists
    SignalMap[callback]->pushback(SlotItem(L, function));
  }
  static int l_disconnect(ScriptRef L) {
    std::string callback = lua_tostring(L, 1);
    std::string function = lua_tostring(L, 2);
    // Remove function from list
  }
};

#endif /* SEAR_SCRIPTENGINE_SIGNAL_H */
