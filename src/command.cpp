// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "ModelLoader.h"

#include "System.h"
#include "cmd.h"
#include "Console.h"
#include "Character.h"
#include "Config.h"
#include "client.h"
#include "Camera.h"
#include "Render.h"
#include "Bindings.h"
#include "Utility.h"

#include <string>
//#include <string.h>
#include "Event.h"
#include "EventHandler.h"

#include <Eris/World.h>
#include <Eris/Entity.h>
#include "WorldEntity.h"
#include <unistd.h>

#include "Log.h"

namespace Sear {


//TODO replace string tokenizer with a better version	
std::string::size_type pos, last_pos;
std::string token_string;
const std::string &delimeters = " ";

void initTokens(const std::string &tokens) {
  token_string = std::string(tokens);
  last_pos = token_string.find_first_not_of(delimeters, 0);
  pos = token_string.find_first_of(delimeters, last_pos);
}

std::string nextToken() {
  try {
    std::string token = token_string.substr(last_pos, pos - last_pos);
    last_pos = token_string.find_first_not_of(delimeters, pos);
    pos = token_string.find_first_of(delimeters, last_pos);
    return token;
  } catch (...) {
    return "";
  }
}

std::string remainingTokens() {
  try {
    return token_string.substr(last_pos, token_string.size() - last_pos);
  } catch (...) {
    return "";
  }
}

void System::runCommand(const std::string &command) {
  int err = 0;
  initTokens(command);	
  std::string toke = nextToken();
  const char *tok = toke.c_str();
  if (toke != CMD_TOGGLE_CONSOLE)  pushMessage(command, CONSOLE_MESSAGE);
  try {
    if (strcasecmp(tok, CMD_QUIT) == 0) _system_running = false;
    else if (strcasecmp(tok, CMD_CONNECT) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = nextToken();
      if (arg2.empty()) {
        if (_client) err = _client->connect(arg1);
        else Log::writeLog("Client not created", Log::ERROR);
      } else {
        int i=0;
	cast_stream(arg2, i);
        if (_client) err = _client->connect(arg1, i);
        else Log::writeLog("Client not created", Log::ERROR);
      }
    }
    else if (strcasecmp(tok, CMD_DISCONNECT) == 0) err = _client->disconnect();
    else if (strcasecmp(tok, CMD_RECONNECT) == 0) err = _client->reconnect();
    else if (strcasecmp(tok, CMD_ACCOUNT_CREATE) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = nextToken();
      std::string arg3 = remainingTokens();
      if (_client) err = _client->createAccount(arg1, arg3, arg2);
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_ACCOUNT_LOGIN) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = nextToken();
      if (_client) err = _client->login(arg1, arg2);
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_ACCOUNT_LOGOUT) == 0) {
      if (_client)  err = _client->logout();
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_CHARACTER_LIST) == 0) {
      if (_client) err = _client->getCharacters();
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_CHARACTER_CREATE) == 0) {
      std::string arg1 = nextToken(); // Name
      std::string arg2 = nextToken(); // Type
      std::string arg3 = nextToken(); // Sex
      std::string arg4 = remainingTokens(); //Description
      if (_client) err = _client->createCharacter(arg1, arg2, arg3, arg4);
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_CHARACTER_TAKE) == 0) {
      std::string arg1 = nextToken();
      if (_client) err = _client->takeCharacter(arg1);
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_STATUS) == 0) {
      if (_client) Log::writeLog(std::string("Status: ") +  _client->getStatus(), Log::DEFAULT);
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_ROOM_LIST) == 0) {
      if (_client) _client->listRooms();
      else Log::writeLog("Client not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_SET_ATTRIBUTE) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = nextToken();
      if (_general) _general->setAttribute(arg1, arg2);
      else Log::writeLog("General config not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_GET_ATTRIBUTE) == 0) {
      std::string arg1 = nextToken();
      if (_general) pushMessage(_general->getAttribute(arg1), CONSOLE_MESSAGE);
      else Log::writeLog("General config not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_SAY) == 0) {
      std::string arg1 = remainingTokens();
      if (_character) _character->say(arg1);
    }

    else if (strcasecmp(tok, CMD_TOGGLE_CONSOLE) ==0) _console->toggleConsole();
    
    else if (strcasecmp(tok, CMD_CAMERA_ZOOM_IN) == 0)       renderer->getCamera()->zoom(-1);
    else if (strcasecmp(tok, CMD_CAMERA_ZOOM_OUT) == 0)      renderer->getCamera()->zoom( 1);
    else if (strcasecmp(tok, CMD_CAMERA_STOP_ZOOM_IN) == 0)  renderer->getCamera()->zoom( 1);
    else if (strcasecmp(tok, CMD_CAMERA_STOP_ZOOM_OUT) == 0) renderer->getCamera()->zoom(-1);

    else if (strcasecmp(tok, CMD_CAMERA_LOOK_LEFT) == 0)       renderer->getCamera()->rotate(-1);
    else if (strcasecmp(tok, CMD_CAMERA_STOP_LOOK_LEFT) == 0)  renderer->getCamera()->rotate( 1);    
    else if (strcasecmp(tok, CMD_CAMERA_LOOK_RIGHT) == 0)      renderer->getCamera()->rotate( 1);
    else if (strcasecmp(tok, CMD_CAMERA_STOP_LOOK_RIGHT) == 0) renderer->getCamera()->rotate(-1);
    
    else if (strcasecmp(tok, CMD_CAMERA_LOOK_UP) == 0)        renderer->getCamera()->elevate( 1);
    else if (strcasecmp(tok, CMD_CAMERA_STOP_LOOK_UP) == 0)   renderer->getCamera()->elevate(-1);
    else if (strcasecmp(tok, CMD_CAMERA_LOOK_DOWN) == 0)      renderer->getCamera()->elevate(-1);
    else if (strcasecmp(tok, CMD_CAMERA_STOP_LOOK_DOWN) == 0) renderer->getCamera()->elevate( 1);
    
    else if (strcasecmp(tok, CMD_CHAR_MOVE_FORW) == 0)      {
      if (_character) _character->moveForward( 1);
    }
    else if (strcasecmp(tok, CMD_CHAR_MOVE_BACK) == 0)      { if (_character) _character->moveForward(-1); }
    else if (strcasecmp(tok, CMD_CHAR_STOP_MOVE_FORW) == 0) { if (_character) _character->moveForward(-1); }
    else if (strcasecmp(tok, CMD_CHAR_STOP_MOVE_BACK) == 0) { if (_character) _character->moveForward( 1); }
   
    else if (strcasecmp(tok, CMD_CHAR_ROT_LEFT) == 0)       { if (_character) _character->rotate(-1); }
    else if (strcasecmp(tok, CMD_CHAR_ROT_RIGHT) == 0)      { if (_character) _character->rotate( 1); }
    else if (strcasecmp(tok, CMD_CHAR_STOP_ROT_LEFT) == 0)  { if (_character) _character->rotate( 1); }
    else if (strcasecmp(tok, CMD_CHAR_STOP_ROT_RIGHT) == 0) { if (_character) _character->rotate(-1); }

    else if (strcasecmp(tok, CMD_CHAR_STRAFE_LEFT) == 0)       { if (_character) _character->strafe(-1); }
    else if (strcasecmp(tok, CMD_CHAR_STRAFE_RIGHT) == 0)      { if (_character) _character->strafe( 1); }
    else if (strcasecmp(tok, CMD_CHAR_STOP_STRAFE_LEFT) == 0)  { if (_character) _character->strafe( 1); }
    else if (strcasecmp(tok, CMD_CHAR_STOP_STRAFE_RIGHT) == 0) { if (_character) _character->strafe(-1); }
    
    else if (strcasecmp(tok, CMD_GIVE) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = remainingTokens();
      int quantity = 0;
      cast_stream(arg1, quantity);
      if (_character) _character->giveEntity(arg2, quantity, renderer->getActiveID());

    } 

    else if (strcasecmp(tok, CMD_PICKUP) == 0) {
      setAction(ACTION_PICKUP);
//      if (_character) _character->getEntity(renderer->getActiveID());
    }
    else if (strcasecmp(tok, CMD_PICKUP2) == 0) {
      std::string arg1 = nextToken();
      if (_character) _character->getEntity(arg1);
    }
    else if (strcasecmp(tok, CMD_TOUCH) == 0) {
      setAction(ACTION_TOUCH);
//      if (_character) _character->touchEntity(renderer->getActiveID());
    }
    else if (strcasecmp(tok, CMD_INVENTORY) == 0) {
      if (_character) _character->displayInventory();
    }
    else if (strcasecmp(tok, CMD_DROP) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = remainingTokens();
      int quantity = 0;
      cast_stream(arg1, quantity);
      if (_character) _character->dropEntity(arg2, quantity);
    }
    else if (strcasecmp(tok, CMD_BIND) == 0) {
      std::string arg1 = nextToken();
      std::string remainder = remainingTokens();      
      Bindings::bind(arg1, remainder);
    }
    else if (strcasecmp(tok, CMD_TOGGLE_FULLSCREEN) == 0) toggleFullscreen();
    else if (strcasecmp(tok, CMD_CALLY_SET_STATE) == 0) {
      std::string arg1 = nextToken();
      int i = atoi(arg1.c_str());
      renderer->setCallyState(i);
    }
    else if (strcasecmp(tok, CMD_CALLY_SET_MOTION) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = nextToken();
      std::string arg3 = nextToken();
      float f1 = atof(arg1.c_str());
      float f2 = atof(arg2.c_str());
      float f3 = atof(arg3.c_str());
      renderer->setCallyMotion(f1, f2, f3);
    }
    else if (strcasecmp(tok, CMD_CALLY_EXECUTE) == 0) {
      int action = atoi(nextToken().c_str());
      renderer->executeCallyAction(action);
    }
    else if (strcasecmp(tok, CMD_KEY_PRESS) == 0) {
      std::string arg1 = nextToken();
      runCommand(Bindings::getBinding(arg1));
    }
    else if (strcasecmp(tok, CMD_EVENT) == 0) {
      std::string arg1 = nextToken();
      std::string arg2 = nextToken();
      std::string arg3 = nextToken();
      std::string arg4 = remainingTokens();
      System::instance()->getEventHandler()->addEvent(Event(arg1, arg4, arg2, arg3));
    }
    else if (strcasecmp(tok, CMD_RUN_SCRIPT) == 0) {
      std::string arg1 = processHome(nextToken());
      runScript(arg1);
    }
    else if (strcasecmp(tok, CMD_LOAD_OBJECT_FILE) == 0) {
      std::string arg1 = processHome(nextToken());
      if (_ol) _ol->readFiles(arg1);
      else Log::writeLog("Object loader not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_LOAD_GENERAL) == 0) {
      std::string arg1 = processHome(nextToken());
      if (_general) _general->loadConfig(arg1);
      else Log::writeLog("General config not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_LOAD_TEXTURE) == 0) {
      std::string arg1 = processHome(nextToken());
      if (_textures) _textures->loadConfig(arg1, _prefix_cwd);
      else Log::writeLog("Textures config not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_LOAD_MODEL) == 0) {
      std::string arg1 = processHome(nextToken());
      if (_models) _models->loadConfig(arg1);
      else Log::writeLog("Models config not created", Log::ERROR);
    }
    else if (strcasecmp(tok, CMD_LOAD_BINDING) == 0) {
      std::string arg1 = processHome(nextToken());
      Bindings::loadBindings(arg1);
    }
    else if (strcasecmp(tok, CMD_SAVE_GENERAL) == 0) {
      std::string arg1 = processHome(nextToken());
      if (_general) {
        if (arg1.empty()) _general->saveConfig();
        else _general->saveConfig(arg1);
      } else {
        Log::writeLog("General config not created", Log::ERROR);
      }
    }
    else if (strcasecmp(tok, CMD_SAVE_BINDING) == 0) {
      std::string arg1 = processHome(nextToken());
      if (arg1.empty()) Bindings::saveBindings();
      else Bindings::saveBindings(arg1);
    }
    else if (strcasecmp(tok, CMD_START_RUN) == 0) {
      if (_character) _character->toggleRunModifier();
    }
    else if (strcasecmp(tok, CMD_STOP_RUN) == 0) {
      if (_character) _character->toggleRunModifier();
    }
    else if (strcasecmp(tok, CMD_TOGGLE_ALWAYS_RUN) == 0) {
      if (_character) _character->toggleRunModifier();
    }
    else if (strcasecmp(tok, CMD_READ_CONFIG) == 0) {
      readConfig();
      if (renderer) {
	renderer->readConfig();
        renderer->readComponentConfig();
      }
      if (_character)_character->readConfig();
    }
    else if (strcasecmp(tok, CMD_IDENTIFY) == 0) {
      Eris::World *world = Eris::World::Instance();
      if (!world) return;
      WorldEntity *we = ((WorldEntity*)(world->lookup(renderer->getActiveID())));
      if (we) we->displayInfo();
    }
    else if (strcasecmp(tok, "lists") == 0) {
      renderer->buildDisplayLists();
      renderer->setupStates();
    }
    else if (strcasecmp(tok, "cd") == 0) {
      std::string dir = remainingTokens();
      if (dir.empty()) return;
      chdir(dir.c_str());
    }
    else if (strcasecmp(tok, "enable_dir_prefix") == 0) {
      _prefix_cwd = true;
    }
    else if (strcasecmp(tok, "disable_dir_prefix") == 0) {
      _prefix_cwd = false;
    }
    else Log::writeLog("Unknown command", Log::ERROR);
  } catch (...) {
    Log::writeLog("Caught unknown exception", Log::ERROR);
  }
//  if (err != 0) traceError(err);
}

}
