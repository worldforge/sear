// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _BINDINGS_H_
#define _BINDINGS_H_ 1

/*
 * This class takes care of the bindings between keys and their associated commands
 * This makes use of a Config object to store its data and it acts as an interface
 * between the system and the config object.
 */ 

#include <string>
#include <map>

namespace Sear {

// Forward Declarations
class Config;

class Bindings {
public:
  /*
   * Resets key bindings back to initial conditions
   */ 	
  static void init();
  /*
   * Cleans up key bindings.
   */ 	  
  static void shutdown();
  /*
   * Merges current bindings with those in file_name
   */ 
  static void loadBindings(const std::string &file_name);
  /*
   * Saves current bindings to last specified file.
   */ 
  static void saveBindings();
  /*
   * Saves current bindings to file_name
   */ 
  static void saveBindings(const std::string &file_name);

  /*
   * Assigns command to the key key_id
   */ 
  static void bind(std::string key_id, std::string command);

  /*
   * Returns the textural representation from given SDL key id
   */ 
  static std::string idToString(int id);
  /*
   * Returns the string bound to the given key id
   */ 
  static std::string getBinding(std::string key_id);

protected:
  /*
   * Sets up the mapping between an SDL key id and a textual representation
   */ 
  static void initKeyMap();
  
  static std::map<int, std::string> keymap; // Mapping storing associations between an SDL key id and a textual representation
  static Config* _bindings; // Config object storing bindings
};

} /* namespace Sear */
#endif /* _BINDINGS_H_ */
