// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: client.h,v 1.13 2003-03-23 19:51:49 simon Exp $

#ifndef SEAR_CLIENT_H
#define SEAR_CLIENT_H 1

#include <string>

#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/Player.h>
#include <Atlas/Message/DecoderBase.h>

#include "conf.h"
#include "interfaces/ConsoleObject.h"

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

namespace Eris {
  class Meta;
  class Player;
  class Connection;
  class Lobby;
  class Person;
  class World;
  class ServerInfo;
}

namespace Sear {
class Factory;
class WorldEntity;
class Console;
class Lobby;
class System;

class Client :public SigC::Object, public ConsoleObject {

public:
  Client(System *system, const std::string &client_name);
  ~Client();

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

  void registerCommands(Console *);
  void runCommand(const std::string &command, const std::string &args);
  void getServers();
  void stopServers();
  
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

  //Metaserver
  void gotServerCount(int count);
  void gotFailure(const std::string& msg);
  void receivedServerInfo(const Eris::ServerInfo & sInfo);
  void completedServerList();

  System *_system;

  Eris::Connection* _connection;
  Eris::Player* _player;
  Eris::Lobby* _lobby;
  Lobby *the_lobby;
  Eris::Meta *_meta;

  Factory *_factory;
  
  int _status;
  std::string _client_name;
  bool _initialised;
};

} /* namespace Sear */
#endif /* SEAR_CLIENT_H */
