// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall, University of Southampton

#ifndef SEAR_CHARACTERMANAGER_H
#define SEAR_CHARACTERMANAGER_H 1

#include <map>
#include <string>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <Atlas/Objects/ObjectsFwd.h>

#include "interfaces/ConsoleObject.h"

namespace Eris {
  class Account;
  class Avatar;
  class Entity;
}

namespace varconf {
  class Config;
}

namespace Sear {

// Forward declarations
class Character;
class Console;

/** Class to manage multiple Character instances.
 * The CharacterManager works around the Eris::Avatar lifecycle and manages
 * character/avatar related tasks. A Character instance is created for each 
 * Avatar object and is destroyed when the avatar is deactivated.
 * Only a single character can be "active" at once. The active character is the
 * focus for rendering and reciving user control commands.
 */
class CharacterManager : public ConsoleObject, public sigc::trackable {
public:
  typedef std::map<std::string, Character*> CharacterMap;

  CharacterManager();
  virtual ~CharacterManager();

  bool init();
  void shutdown();

  bool isInitialised() const { return m_initialised; }

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config) const;

  void registerCommands(Console*);
  void runCommand(const std::string &, const std::string &);

  /**
   *  Returns a mapping between avatar id and Character objects.
   */ 
  const CharacterMap &getCharacterMap() const {
    return m_character_map;
  }

  /**
   * Make the specified character object the active character.
   * This object must be contained within the character map.
   */ 
  bool setActiveCharacter(Character *character);

  /**
   * Return a pointer to the currently active character.
   * This may be null.
   */  
  Character *getActiveCharacter() const {
    return m_active_character;
  }

  /**
   * Signal fired when the active character changes. The character pointer may
   * be null when there are no more active characters.
   */ 
  sigc::signal<void, Character*> ActiveCharacterChanged;

  /**
   * Use the Eris::Account object to obtain Avatars.
   *
   */  
  void setAccount(Eris::Account *);

private:
  bool m_initialised; 

  /**
   * Mapping between avatar id and Character instance.
   */ 
  CharacterMap m_character_map;

  /**
   * Pointer to currently active character
   */ 
  Character *m_active_character;

  /**
   * Callback for Avatar creation.
   */ 
  void AvatarSuccess(Eris::Avatar *);

  /**
   * Callback for Avatar deactivation.
   */ 
  void AvatarDeactivated(Eris::Avatar* av);

  /**
   * Callback when Avatar Entity has been created
   */ 
  void GotCharacterEntity(Eris::Entity *e, Eris::Avatar *avatar);

  /**
   * Method to trigger character list retrieval from server..
   */  
  int getCharacters();

  /**
   * Method to request character creation on the server
   */ 
  int createCharacter(const std::string &, const std::string &, const std::string&, const std::string &);

  /**
   * Method to activate an existing character
   */ 
  int takeCharacter(const std::string &);

  /**
   * Callback when an existing character object has been reported after a 
   * getCharacters() call.
   */ 
  void GotCharacterInfo(const Atlas::Objects::Entity::RootEntity&);

  /**
   * Callback fired when all existing characters have been reported after a
   * getCharacters() call.
   */
  void GotAllCharacters();

  bool m_take_first;
  Eris::Account *m_account;
};

} // namespace Sear 

#endif // SEAR_CHARACTERMANAGER_H 
