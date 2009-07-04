// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall, University of Southampton

#include "src/CharacterManager.h"

#include <math.h>
#include <string>
#include <SDL/SDL.h>

#include <sigc++/object_slot.h>
#include <sigc++/bind.h>
#include <varconf/config.h>

#include <Atlas/Objects/Entity.h>


#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/View.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "ActionHandler.h"
#include "client.h"
#include "System.h"
#include "Character.h"
#include "Console.h"
#include "WorldEntity.h"

#include "renderers/RenderSystem.h"
#include "renderers/Render.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

// Console commands
static const std::string CHARACTER_LIST = "get_character_list";
static const std::string CHARACTER_CREATE = "add";
static const std::string CHARACTER_TAKE = "take";

static const std::string CMD_MOVE_FORWARD = "+character_move_forward";
static const std::string CMD_MOVE_BACKWARD = "+character_move_backward";
static const std::string CMD_MOVE_STOP_FORWARD = "-character_move_forward";
static const std::string CMD_MOVE_STOP_BACKWARD = "-character_move_backward";

static const std::string CMD_MOVE_UPWARD = "+character_move_upwards";
static const std::string CMD_MOVE_DOWNWARD = "+character_move_downwards";
static const std::string CMD_MOVE_STOP_UPWARD = "-character_move_upwards";
static const std::string CMD_MOVE_STOP_DOWNWARD = "-character_move_downwards";

static const std::string CMD_ROTATE_LEFT = "+character_rotate_left";
static const std::string CMD_ROTATE_RIGHT = "+character_rotate_right";
static const std::string CMD_ROTATE_STOP_LEFT = "-character_rotate_left";
static const std::string CMD_ROTATE_STOP_RIGHT = "-character_rotate_right";

static const std::string CMD_STRAFE_LEFT = "+character_strafe_left";
static const std::string CMD_STRAFE_RIGHT = "+character_strafe_right";
static const std::string CMD_STRAFE_STOP_LEFT = "-character_strafe_left";
static const std::string CMD_STRAFE_STOP_RIGHT = "-character_strafe_right";

static const std::string CMD_RUN = "+run";
static const std::string CMD_STOP_RUN = "-run";
static const std::string CMD_TOGGLE_RUN = "toggle_run";

static const std::string CMD_SAY = "say";
static const std::string CMD_ME = "me";
static const std::string CMD_PICKUP = "pickup";
static const std::string CMD_TOUCH = "touch";
static const std::string CMD_DROP = "drop";
static const std::string CMD_EAT = "eat";
static const std::string CMD_GIVE = "give";
static const std::string CMD_DISPLAY_INVENTORY = "inventory";
static const std::string CMD_MAKE = "make";
static const std::string CMD_USE = "use";
static const std::string CMD_IDLE = "idle";
static const std::string CMD_WIELD = "wield";
static const std::string CMD_ATTACK = "attack";
static const std::string CMD_DISPLAY_USE_OPS = "use_ops";
  
static const std::string CMD_SET_HEIGHT = "set_height";
static const std::string CMD_RENAME_ENTITY = "rename_entity";

static const std::string CMD_MOVE_TO = "move_to";
static const std::string CMD_MOVE_TO_ORIGIN = "return_to_origin";

static const unsigned int server_update_interval = 500;

static const std::string STOPPED = "ch_stopped_";
static const std::string WALKING = "ch_walking_";
static const std::string RUNNING = "ch_running_";

static const std::string HEIGHT = "height";

static const std::string CMD_SWITCH_CHARACTER = "switch_character";
namespace Sear {

CharacterManager::CharacterManager() :
  m_initialised(false),
  m_active_character(NULL),
  m_take_first(false),
  m_account(NULL)
{
}

CharacterManager::~CharacterManager() {
  if (m_initialised) shutdown();
}

bool CharacterManager::init() {
  assert (m_initialised == false);

  m_initialised = true;

  return true;
}

void CharacterManager::shutdown() {
  assert (m_initialised == true);

  notify_callbacks();

  m_initialised = false;
}

void CharacterManager::readConfig(varconf::Config &config) {

  CharacterMap::iterator I = m_character_map.begin();
  CharacterMap::iterator Iend = m_character_map.end();

  while (I != Iend) {
     Character *c = (I++)->second;
     c->readConfig(config);
  }

  // TODO: Store config for use when creating characters

}

void CharacterManager::writeConfig(varconf::Config &config) const {

}

void CharacterManager::registerCommands(Console *console) {
  console->registerCommand(CMD_MOVE_FORWARD, this);
  console->registerCommand(CMD_MOVE_BACKWARD, this);
  console->registerCommand(CMD_MOVE_STOP_FORWARD, this);
  console->registerCommand(CMD_MOVE_STOP_BACKWARD, this);

  console->registerCommand(CMD_MOVE_UPWARD, this);
  console->registerCommand(CMD_MOVE_DOWNWARD, this);
  console->registerCommand(CMD_MOVE_STOP_UPWARD, this);
  console->registerCommand(CMD_MOVE_STOP_DOWNWARD, this);

  console->registerCommand(CMD_ROTATE_LEFT, this);
  console->registerCommand(CMD_ROTATE_RIGHT, this);
  console->registerCommand(CMD_ROTATE_STOP_LEFT, this);
  console->registerCommand(CMD_ROTATE_STOP_RIGHT, this);

  console->registerCommand(CMD_STRAFE_LEFT, this);
  console->registerCommand(CMD_STRAFE_RIGHT, this);
  console->registerCommand(CMD_STRAFE_STOP_LEFT, this);
  console->registerCommand(CMD_STRAFE_STOP_RIGHT, this);

  console->registerCommand(CMD_RUN, this);
  console->registerCommand(CMD_STOP_RUN, this);
  console->registerCommand(CMD_TOGGLE_RUN, this);

  console->registerCommand(CMD_PICKUP, this);
  console->registerCommand(CMD_DROP, this);
  console->registerCommand(CMD_EAT, this);
  console->registerCommand(CMD_GIVE, this);
  console->registerCommand(CMD_DISPLAY_INVENTORY, this);
  console->registerCommand(CMD_MAKE, this);
  console->registerCommand(CMD_ME, this);
  console->registerCommand(CMD_TOUCH, this);
  console->registerCommand(CMD_SAY, this);
  console->registerCommand(CMD_USE, this);
  console->registerCommand(CMD_IDLE, this);
  console->registerCommand(CMD_WIELD, this);
  console->registerCommand(CMD_ATTACK, this);
  console->registerCommand(CMD_SET_HEIGHT, this);
  console->registerCommand(CMD_DISPLAY_USE_OPS, this);
  console->registerCommand(CMD_RENAME_ENTITY, this);
  console->registerCommand(CMD_MOVE_TO, this);
  console->registerCommand(CMD_MOVE_TO_ORIGIN, this);

  console->registerCommand(CMD_SWITCH_CHARACTER, this);
  console->registerCommand(CHARACTER_LIST, this);
  console->registerCommand(CHARACTER_CREATE, this);
  console->registerCommand(CHARACTER_TAKE, this);
 
}

void CharacterManager::runCommand(const std::string &command, const std::string &args) {
  assert ((m_initialised == true) && "Character not initialised");

  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(args);

  if (command == CMD_SWITCH_CHARACTER) {
    const std::string &idx_str = tokeniser.nextToken();
    int idx;
    cast_stream(idx_str, idx);
  
    printf("Idx %d (%d)\n", idx, m_character_map.size());
 
    CharacterMap::const_iterator I = m_character_map.begin();
    CharacterMap::const_iterator Iend = m_character_map.end();
    int c = 0;
    while (I != Iend) {
    printf("c%c (%d)\n", idx, m_character_map.size());
      if (c == idx) {
printf("Found item\n");
        setActiveCharacter(I->second);
        return;
      }
      ++c;
      ++I;
    }
    // TODO: Warn invalid index
    fprintf(stderr, "[CharacterManager] Selected index out of range\n");
 
  }

  else if (command == CHARACTER_LIST) {
    getCharacters();
  }
  else if (command == CHARACTER_CREATE) {
    std::string name = tokeniser.nextToken();
    std::string type = tokeniser.nextToken();
    std::string sex = tokeniser.nextToken();
    std::string desc = tokeniser.remainingTokens();
    createCharacter(name, type, sex, desc);
  }
  else if (command == CHARACTER_TAKE) {
    takeCharacter(args);
  }

  if (!m_active_character) return;

  Eris::Avatar *avatar = m_active_character->getAvatar();

  if (!avatar) {
    return;
  }


  if (command == CMD_MOVE_FORWARD) m_active_character->moveForward(1);
  else if (command == CMD_MOVE_BACKWARD) m_active_character->moveForward(-1);
  else if (command == CMD_MOVE_STOP_FORWARD) m_active_character->moveForward(-1);
  else if (command == CMD_MOVE_STOP_BACKWARD) m_active_character->moveForward( 1);

  else if (command == CMD_MOVE_UPWARD) m_active_character->moveUpward(1);
  else if (command == CMD_MOVE_DOWNWARD) m_active_character->moveUpward(-1);
  else if (command == CMD_MOVE_STOP_UPWARD) m_active_character->moveUpward(-1);
  else if (command == CMD_MOVE_STOP_DOWNWARD) m_active_character->moveUpward( 1);

  else if (command == CMD_ROTATE_LEFT) m_active_character->rotate( 1);
  else if (command == CMD_ROTATE_RIGHT) m_active_character->rotate(-1);
  else if (command == CMD_ROTATE_STOP_LEFT) m_active_character->rotate(-1);
  else if (command == CMD_ROTATE_STOP_RIGHT) m_active_character->rotate( 1);

  else if (command == CMD_STRAFE_LEFT) m_active_character->strafe( 1);
  else if (command == CMD_STRAFE_RIGHT) m_active_character->strafe(-1);
  else if (command == CMD_STRAFE_STOP_LEFT) m_active_character->strafe(-1);
  else if (command == CMD_STRAFE_STOP_RIGHT) m_active_character->strafe( 1);

  else if (command == CMD_RUN || command == CMD_STOP_RUN || command == CMD_TOGGLE_RUN) m_active_character->toggleRunModifier();

  else if (command == CMD_SAY) m_active_character->say(args);
  else if (command == CMD_ME) m_active_character->emote(args);
  else if (command == CMD_GIVE) {
    const std::string &quantity_str = tokeniser.nextToken();
    const std::string &item = tokeniser.remainingTokens();
    int quantity = 0;
    cast_stream(quantity_str, quantity);
    m_active_character->giveEntity(item, quantity, RenderSystem::getInstance().getRenderer()->getActiveID());
  }
  else if (command == CMD_DROP) {
    const std::string &quantity_str = tokeniser.nextToken();
    const std::string &item = tokeniser.remainingTokens();
    int quantity = 0;
    cast_stream(quantity_str, quantity);
    m_active_character->dropEntity(item, quantity);
  }
  else if (command == CMD_PICKUP) System::instance()->setAction(ACTION_PICKUP);
  else if (command == CMD_TOUCH) System::instance()->setAction(ACTION_TOUCH);
  else if (command == CMD_DISPLAY_INVENTORY) {
//    displayInventory();
    System::instance()->getActionHandler()->handleAction("inventory_open", NULL);
  }
  else if (command == CMD_MAKE) {
    const std::string &type = tokeniser.nextToken();
    std::string name = tokeniser.remainingTokens();
    if (name.empty()) {
        name = type;
    }
    m_active_character->make(type, name);
  }
  else if (command == CMD_USE) {
    System::instance()->setAction(ACTION_USE);
//    useToolOnEntity(RenderSystem::getInstance().getRenderer()->getActiveID());
  }
  else if (command == CMD_IDLE) {
    m_active_character->becomeIdle();
  }
  else if (command == CMD_DISPLAY_USE_OPS) {
    m_active_character->displayUseOperations();
  }
  else if (command == CMD_WIELD) {
    const std::string &name = tokeniser.nextToken();
    m_active_character->wieldEntity(name);
  }
  else if (command == CMD_EAT) {
    const std::string &name = tokeniser.nextToken();
    m_active_character->eatEntity(name);
  }
  else if (command == CMD_ATTACK) {
    System::instance()->setAction(ACTION_ATTACK);
  }
  else if (command == CMD_SET_HEIGHT) {
    const std::string &hStr = tokeniser.nextToken();
    float h;
    cast_stream(hStr, h);
    m_active_character->setHeight(h);
  }
  else if (command ==  CMD_RENAME_ENTITY) {
    const std::string &id = tokeniser.nextToken();
    const std::string &name = tokeniser.remainingTokens();
    WorldEntity *we = m_active_character->findInInventory(id);
    if (we) m_active_character->renameEntity(we, name);
  }
  else if (command == CMD_MOVE_TO_ORIGIN) {
    avatar->moveToPoint(WFMath::Point<3>(0,0,0));
  }
  else if (command == CMD_MOVE_TO) {

    const std::string &str_x = tokeniser.nextToken();
    const std::string &str_y = tokeniser.nextToken();
    const std::string &str_z = tokeniser.nextToken();
    float x,y,z;
    cast_stream(str_x, x);
    cast_stream(str_y, y);
    cast_stream(str_z, z);
    avatar->moveToPoint(WFMath::Point<3>(x,y,z));
  }


}

void CharacterManager::setAccount(Eris::Account *account) {
   // Setup callbacks
  printf("[CharacterManager] ssetAccount\n");
  account->AvatarSuccess.connect(sigc::mem_fun(this, &CharacterManager::AvatarSuccess));
  account->AvatarDeactivated.connect(sigc::mem_fun(this, &CharacterManager::AvatarDeactivated));
  account->GotAllCharacters.connect(sigc::mem_fun(this, &CharacterManager::GotAllCharacters));
  account->GotCharacterInfo.connect(sigc::mem_fun(this, &CharacterManager::GotCharacterInfo));
 
  m_account = account; 
}

void CharacterManager::AvatarSuccess(Eris::Avatar *avatar) {
  assert(avatar != NULL);

  printf("[CharacterManager] AvatarSucess\n");
  // Hook up signal to inidicate when the char entity exists
  avatar->GotCharacterEntity.connect(sigc::bind(sigc::mem_fun(this, &CharacterManager::GotCharacterEntity), avatar));
}

void CharacterManager::AvatarDeactivated(Eris::Avatar* av)
{

  printf("[CharacterManager] AvatarDeactivated\n");

  // Find the character instance
  CharacterMap::iterator I = m_character_map.find(av->getId());
  Character *c = I->second;

  // Remove it from the map
  m_character_map.erase(I);

  // If character count is now 0, update status
  if (m_character_map.size() == 0) {
    setActiveCharacter(NULL);
  } else {
    if (m_active_character == c) {
      // Make first character the currently active one if we just 
      // deactivated the active character.
      setActiveCharacter(m_character_map.begin()->second);
    }  
  }

  // Clean up
  c->shutdown();
  delete c;
}

void CharacterManager::GotCharacterEntity(Eris::Entity *e, Eris::Avatar *avatar) {
  assert(e != NULL);
  assert(avatar != NULL);

  printf("[CharacterManager] GotCharacterEntity\n");

  // Mark Character as valid for use
  Character *c = new Character();
  c->init(avatar);

  // TODO: Should be ref stored from readConfig
  c->readConfig(System::instance()->getGeneral());

  m_character_map[avatar->getId()] = c;

  // If character count is now 1, update status
//  if (m_character_map.size() == 1) {
    // TODO: We may want to always make new characters the active one?
    setActiveCharacter(c);
//  }
}

bool CharacterManager::setActiveCharacter(Character *c) {

  printf("[CharaacterManager] setActiveCharacter\n");
  m_active_character = c;

  ActiveCharacterChanged.emit(c);  

  return true;
}

int CharacterManager::createCharacter(const std::string &name, const std::string &type, const std::string &sex, const std::string &description)
{
  assert (m_initialised == true);
printf("[CharacterManager] Create Character\n");
  assert(m_account != NULL);

  if (name.empty()) {
    System::instance()->pushMessage("Error: No character name specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (type.empty()) {
    System::instance()->pushMessage("Error: No character type specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (sex.empty()) {
    System::instance()->pushMessage("Error: No character gender specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (description.empty()) {
    System::instance()->pushMessage("Error: No character description specified", CONSOLE_MESSAGE);
    return 1;
  }

  if  (debug) printf("[CharacterManager] Creating character - Name: %s Type: %s Sex: %s Description: %s\n", name.c_str(), type.c_str(), sex.c_str(), description.c_str());

  System::instance()->pushMessage("Creating Character: " +  name, CONSOLE_MESSAGE);

  // Create atlas character object
  Atlas::Objects::Entity::GameEntity ch;
  ch->setParents(std::list<std::string>(1, type));
  ch->setName(name);
  ch->setAttr("sex", sex);
  ch->setAttr("description", description);

  Eris::Result res = m_account->createCharacter(ch);

  switch (res) {
    case Eris::NO_ERR: break;
    case Eris::DUPLICATE_CHAR_ACTIVE:
      fprintf(stderr, "[CharacterManager] Character is already in use.\n");
      System::instance()->pushMessage("Character is already in use.",
                            CONSOLE_MESSAGE | SCREEN_MESSAGE);
      return 1;
      break;
    case Eris::BAD_CHARACTER_ID:
    case Eris::ALREADY_LOGGED_IN:
    case Eris::NOT_CONNECTED:
    case Eris::NOT_LOGGED_IN:
      assert(false);
      fprintf(stderr, "[CharacterManager] We've hit an unexpected return code %d\n", res);
      System::instance()->pushMessage("An Unknown error occured",
                            CONSOLE_MESSAGE | SCREEN_MESSAGE);
      return 1;
      break;
  }
  return 0;
}

int CharacterManager::takeCharacter(const std::string &id) {

  // TODO: We can hit an Eris exception when taking the same char more than once.
  //       However there is a DUPLICATE_CHAR_ACTIVE return code...

  assert (m_initialised == true);

  assert (m_account != NULL);

  if (id.empty()) {
    m_take_first = true;
    getCharacters();
    return 0;
  }

  if (debug) printf("[CharacterManager] Taking character - %s\n", id.c_str());
  System::instance()->pushMessage(std::string(CLIENT_TAKE_CHARACTER) + std::string(": ") + id, CONSOLE_MESSAGE);

  Eris::Result res = m_account->takeCharacter(id);

  switch (res) {
    case Eris::NO_ERR: break;
    case Eris::BAD_CHARACTER_ID:
      break;
      fprintf(stderr, "[CharacterManager] Bad character ID\n");
      System::instance()->pushMessage("Bad character ID",
                            CONSOLE_MESSAGE | SCREEN_MESSAGE);
      return 1;
    case Eris::DUPLICATE_CHAR_ACTIVE:
      fprintf(stderr, "[CharacterManager] Character is already in use.\n");
      System::instance()->pushMessage("Character is already in use.",
                            CONSOLE_MESSAGE | SCREEN_MESSAGE);
      return 1;
      break;
    case Eris::ALREADY_LOGGED_IN:
    case Eris::NOT_CONNECTED:
    case Eris::NOT_LOGGED_IN:
      assert(false);
      fprintf(stderr, "[CharacterManager] We've hit an unexpected return code\n");
      System::instance()->pushMessage("An Unknown error occured",
                            CONSOLE_MESSAGE | SCREEN_MESSAGE);
      return 1;
      break;
  }
  return 0;
}

int CharacterManager::getCharacters() {
  assert (m_initialised == true);
  if (m_account) {
    m_account->refreshCharacterInfo();
  } else {
    System::instance()->pushMessage("Error: Not logged in", CONSOLE_MESSAGE);
  }
  return 0;
}


void CharacterManager::GotCharacterInfo(const Atlas::Objects::Entity::RootEntity& ge) {
  if (debug) printf("[CharacterManager] Got Char - Name: %s ID: %s\n", ge->getName().c_str(), ge->getId().c_str());
}


void CharacterManager::GotAllCharacters() {

  assert(m_account);

  Eris::CharacterMap m = m_account->getCharacters();
  for (Eris::CharacterMap::const_iterator I = m.begin(); I != m.end(); ++I) {
    System::instance()->pushMessage(I->first.c_str(), CONSOLE_MESSAGE);
    if (m_take_first) {
      takeCharacter(I->first.c_str());
      m_take_first = false;
      return;
    }
  }
}



} /* namespace Sear */
