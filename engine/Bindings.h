// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: Bindings.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_ENGINE_BINDINGS_H
#define SEAR_ENGINE_BINDINGS_H 1

#include <string>
#include <map>

extern "C" {
#include <lua.h>
}

// Forward Declarations
namespace varconf {
  class Config;
}

namespace Sear {

/**
 * This class takes care of the bindings between keys and their associated
 * commands. This makes use of a Config object to store its data and it
 * acts as an interface between the system and the config object.
 */ 


class Bindings {
public:
  /**
   * Resets key bindings back to initial conditions
   */ 	
  static void init();
  /**
   * Cleans up key bindings.
   */ 	  
  static void shutdown();
  /**
   * Merges current bindings with those in file_name
   */ 
  static void loadBindings(const std::string &file_name);
  /**
   * Saves current bindings to file_name
   */ 
  static void saveBindings(const std::string &file_name);

  /**
   * Assigns command to the key key_id
   */ 
  static void bind(std::string key_id, std::string command);

  /**
   * Returns the textural representation from given SDL key id
   */ 
  static std::string idToString(int id);
  /**
   * Returns the string bound to the given key id
   */ 
  static std::string getBinding(const std::string &key_id);

  /**
   * Register ScriptEngine commands
   */ 
  static void registerCommands(lua_State *L);
  
  /**
   * l_bind SE command binds a key to a binding.
   */ 
  static int l_bind(lua_State *L);
  
protected:
  /**
   * Sets up the mapping between an SDL key id and a textual representation
   */ 
  static void initKeyMap();
  
  static std::map<int, std::string> keymap; // Mapping storing associations between an SDL key id and a textual representation
  static varconf::Config* _bindings; // Config object storing bindings
};

} /* namespace Sear */
#endif /* SEAR_ENGINE_BINDINGS_H */
