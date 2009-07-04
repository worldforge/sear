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

  const CharacterMap &getCharacterMap() const {
    return m_character_map;
  }

  bool setActiveCharacter(Character *character);

  Character *getActiveCharacter() const {
    return m_active_character;
  }

  sigc::signal<void, Character*> ActiveCharacterChanged;

  void setAccount(Eris::Account *);

private:
  bool m_initialised; 

  CharacterMap m_character_map;

  Character *m_active_character;

  void AvatarSuccess(Eris::Avatar *);
  void AvatarDeactivated(Eris::Avatar* av);

  void GotCharacterEntity(Eris::Entity *e, Eris::Avatar *avatar);


  int getCharacters();
  int createCharacter(const std::string &, const std::string &, const std::string&, const std::string &);
  int takeCharacter(const std::string &);

  void GotCharacterInfo(const Atlas::Objects::Entity::RootEntity&);
  void GotAllCharacters();

  bool m_take_first;
  Eris::Account *m_account;
};

} /* namespace Sear */

#endif /* SEAR_CHARACTERMANAGER_H */
