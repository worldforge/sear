// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CLIENT_H_
#define _CLIENT_H_ 1

#include "conf.h"
#include <Eris/Connection.h>
#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Log.h>
#include <list>
#include <map>
#include <string>

#include <Atlas/Message/DecoderBase.h>

#include "debug.h"


//Client Messages
#define CLIENT_CONNECTING       "Connecting"
#define CLIENT_CONNECTED        "Connected"
#define CLIENT_RECONNECTING     "Reconnecting"
#define CLIENT_DISCONNECTING    "Disconnecting"
#define CLIENT_DISCONNECTED     "Disconnected"
#define CLIENT_LOGGING_IN       "Logging In"
#define CLIENT_LOGGED_IN        "Logged In"
#define CLIENT_LOGGING_OUT      "Logging Out"
#define CLIENT_LOGGED_OUT       "Logged Out"
#define CLIENT_CREATE_CHARACTER "Creating Character"
#define CLIENT_TAKE_CHARACTER   "Taking Character"
#define CLIENT_SIGHTED_ROOM     "Sighted Room"
#define CLIENT_SIGHTED_PERSON   "Sighted Person"

#define CLIENT_TIMEOUT          "Timeout"
#define CLIENT_STATUS_CHANGED   "Status Changed"


#define CLIENT_NETWORK_FAILURE "Network Failure"
#define CLIENT_ERIS_LOG_HEAD   "Eris Log:"

#define CLIENT_STATUS_DISCONNECTED (0)
#define CLIENT_STATUS_CONNECTED    (1)
#define CLIENT_STATUS_LOGGED_IN    (2)
#define CLIENT_STATUS_IN_WORLD     (3)

namespace Sear {

class WorldEntity;
class Console;
class System;

class Client :public SigC::Object {

public:
  Client(System *system, const std::string &client_name);
  ~Client() {
  }

  bool init();
  void shutdown();
  
  int connect(const std::string &, int port = DEFAULT_PORT);
  int reconnect();
  int disconnect();

  int createAccount(const std::string &, const std::string &, const std::string &);
  int login(const std::string &, const std::string &);
  int logout();

  int getCharacters();	
  int createCharacter(const std::string &, const std::string &, const std::string&, const std::string &);
  int takeCharacter(const std::string &);
 
  void poll();
  std::string getStatus();
  
  int listRooms();

  void updateSystem();
  
protected:
  //Callbacks

  //Connection
  void NetFailure(const std::string&);
  void NetConnected();
  void NetDisconnected();
  bool NetDisconnecting();
  void Timeout(Eris::Connection::Status);
  void StatusChanged(Eris::Connection::Status);
  void Log(Eris::LogLevel, const std::string &);

  //Lobby
  void LoggedIn(const Atlas::Objects::Entity::Player &p);
  void SightPerson(Eris::Person*);

  //Player
  void LoginSuccess();
  void LoginFailure(Eris::LoginFailureType, const std::string&);
  void LogoutComplete(bool);
  void GotCharacterInfo(const Atlas::Objects::Entity::GameEntity&);
  void GotAllCharacters();

  //World	
  void EntityCreate(Eris::Entity*);
  void EntityDelete(Eris::Entity*);
  void Entered(Eris::Entity*);
  void Appearance(Eris::Entity*);
  void Disappearance(Eris::Entity*);
  void RootEntityChanged(Eris::Entity*);

  System *_system;

  Eris::Connection* _connection;
  Eris::Player* _player;
  Eris::Lobby* _lobby;

  int _status;
  std::string _client_name;
};

} /* namespace Sear */
#endif /* _CLIENT_H_ */
