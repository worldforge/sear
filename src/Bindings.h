// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall, University of Southampton

// $Id: Bindings.h,v 1.7 2003-03-06 23:50:38 simon Exp $

#ifndef SEAR_BINDINGS_H
#define SEAR_BINDINGS_H 1

/*
 * This class takes care of the bindings between keys and their associated commands
 * This makes use of a Config object to store its data and it acts as an interface
 * between the system and the config object.
 */ 

#include <string>
#include <map>

// Forward Declarations
namespace varconf {
  class Config;
}

/**
 * Sear namespace
 */ 
namespace Sear {

/**
 * This class maintains the mappings between keys and functions.
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
   * @param file_name Name of a key bindings file
   */
  static void loadBindings(const std::string &file_name);
  
  /**
   * Saves current bindings to file_name
   * @param file_name Name of file to save key bindings to
   */ 
  static void saveBindings(const std::string &file_name);

  /**
   * Assigns command to the key key_id
   * @param key_id Key identifier string
   * @param command Command string
   */ 
  static void bind(std::string key_id, std::string command);

  /*
   * Returns the string representation from given SDL key id
   * @param id SDL key ID
   * @return String representation
   */ 
  static std::string idToString(int id);
  
  /*
   * Returns the string bound to the given key id
   * @key_id String representation of key
   * @return Command string
   */ 
  static std::string getBinding(const std::string &key_id);

protected:
  
  /*
   * Sets up the mapping between an SDL key id and a textual representation
   */ 
  static void initKeyMap();
  
  static std::map<int, std::string> keymap; /**< Mapping storing associations between an SDL key id and a textual representation */
  static varconf::Config* _bindings; /**< Config object storing bindings */
};

} /* namespace Sear */

#endif /* SEAR_BINDINGS_H */
