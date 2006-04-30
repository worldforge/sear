// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: client.h,v 1.25 2006-04-30 18:13:41 alriddoch Exp $

#ifndef SEAR_CLIENT_H
#define SEAR_CLIENT_H 1

#include <string>

#include <Eris/Connection.h>
#include <Eris/Log.h>
#include "interfaces/ConsoleObject.h"

#include <sigc++/trackable.h>

#include "common/SPtr.h"

namespace varconf {
class Config;
}

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

typedef enum {
  CLIENT_STATUS_DISCONNECTED = 0,
  CLIENT_STATUS_CONNECTING,
  CLIENT_STATUS_DISCONNECTING,
  CLIENT_STATUS_CONNECTED,
  CLIENT_STATUS_LOGGING_IN,
  CLIENT_STATUS_LOGGING_OUT,
  CLIENT_STATUS_LOGGED_IN,
  CLIENT_STATUS_GOING_IN_WORLD,
  CLIENT_STATUS_GOING_OUT_WORLD,
  CLIENT_STATUS_IN_WORLD,
  CLIENT_STATUS_LAST
} ClientStatus;

namespace Eris {
  class Account;
  class Connection;
  class Avatar;
}

namespace Sear {
class Factory;
class Console;
class System;

class Client : public sigc::trackable, public ConsoleObject {

public:
  Client(System *system, const std::string &client_name);
  ~Client();

  bool init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }
  
  int connect(const std::string &, int port = 6767);
  int disconnect();

  int createAccount(const std::string &, const std::string &, const std::string &);
  int login(const std::string &, const std::string &);
  int logout();

  int getCharacters();	
  int createCharacter(const std::string &, const std::string &, const std::string&, const std::string &);
  int takeCharacter(const std::string &);
 
  void poll();
  std::string getStatus();
  
  void registerCommands(Console *);
  void runCommand(const std::string &command, const std::string &args);
 
  Eris::Account    *getAccount() { return m_account.get(); }
  Eris::Connection *getConnection() { return m_connection.get(); }
  Eris::Avatar     *getAvatar() const { return m_avatar; }

  void readConfig(varconf::Config &config);
  void writeConfig(varconf::Config &config) const;
 
protected:
  void setStatus(int status);
  void setErisLogLevel(const std::string &level);
  //Callbacks

  //Connection
  void NetFailure(const std::string&);
  void NetConnected();
  void NetDisconnected();
  bool NetDisconnecting();
  void StatusChanged(Eris::Connection::Status);
  void Log(Eris::LogLevel, const std::string &);

  // Account
  void LoginSuccess();
  void LoginFailure(const std::string&);
  void LogoutComplete(bool);
  void GotCharacterInfo(const Atlas::Objects::Entity::RootEntity&);
  void GotAllCharacters();
  void AvatarSuccess(Eris::Avatar *);
  void AvatarFailure(const std::string &msg);


  void GotCharacterEntity(Eris::Entity *e);

  System *m_system;

  SPtr<Eris::Connection> m_connection;
  SPtr<Eris::Account>    m_account;

  Eris::Avatar *m_avatar;

  int m_status;
  std::string m_client_name;
  bool m_initialised;
  bool m_takeFirst;

  Eris::LogLevel m_loglevel;
};

} /* namespace Sear */
#endif /* SEAR_CLIENT_H */
