// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: client.cpp,v 1.68 2005-04-25 22:47:22 jmt Exp $

#include "System.h"

#include <sigc++/object.h>
#include <sigc++/slot.h>
#include <sigc++/object_slot.h>
#include <sigc++/bind.h>


#include <Atlas/Objects/Entity.h>

#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/DeleteLater.h>
#include <Eris/Log.h>
#include <Eris/Entity.h>
#include <Eris/Person.h>
#include <Eris/PollDefault.h>
#include <Eris/Metaserver.h>
#include <Eris/ServerInfo.h>
#include <Eris/Exceptions.h>
#include <Eris/View.h>

#include "common/Utility.h"

#include "ActionHandler.h"
#include "client.h"
#include "Console.h"
#include "Factory.h"
#include "Character.h"
#include "renderers/Render.h"
#include "loaders/Model.h"
#include "loaders/ObjectHandler.h"
#include "loaders/ObjectRecord.h"
#include "Exception.h"
#include "WorldEntity.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
#ifdef DEBUG
  #define DEBUG_ERIS 0
  #define ERIS_LOG_LEVEL 0
#elif defined(NDEBUG)
  #define DEBUG_ERIS 0
  #define ERIS_LOG_LEVEL 0
#else
  #define DEBUG_ERIS 0
  #define ERIS_LOG_LEVEL 0
#endif

namespace Sear {

static const int DEFAULT_PORT = 6767;

// Console Command strings
static const std::string SERVER_CONNECT = "connect";
static const std::string SERVER_DISCONNECT = "disconnect";
static const std::string ACCOUNT_CREATE = "create";
static const std::string ACCOUNT_LOGIN = "login";
static const std::string ACCOUNT_LOGOUT = "logout";
static const std::string CHARACTER_LIST = "get";
static const std::string CHARACTER_CREATE = "add";
static const std::string CHARACTER_TAKE = "take";
static const std::string GET_SERVERS = "get_servers";
static const std::string STOP_SERVERS = "stop_servers";


/*
 * The Constructor. Creates the initial Eris Connection object and
 * 
 * @param client_name - Name of client to pass to Eris
*/
Client::Client(System *system, const std::string &client_name) :
  m_system(system),
  m_connection(NULL),
  m_account(NULL),
  m_avatar(NULL),
  m_factory(NULL),
  m_status(CLIENT_STATUS_DISCONNECTED),
  m_client_name(client_name),
  m_initialised(false),
  m_takeFirst(false)
{
  assert((system != NULL) && "System is NULL");
}

Client::~Client() {
  assert(m_initialised == false);
}

bool Client::init() {
  assert(m_initialised == false);

  // Setup logging
  Eris::setLogLevel((Eris::LogLevel) ERIS_LOG_LEVEL);
  Eris::Logged.connect(SigC::slot(*this, &Client::Log));

  m_initialised = true;

  return true;
}

void Client::shutdown() {
  assert(m_initialised == true);

  if (m_account) {
    delete m_account;
    m_account = NULL;
  }

  if (m_connection) {
    delete m_connection;
    m_connection = NULL;
  }

//  if (m_factory) {
//    delete m_factory;
//    m_factory = NULL;
//  }

  setStatus(CLIENT_STATUS_DISCONNECTED);

  m_initialised = false;
}

void Client::Log(Eris::LogLevel lvl, const std::string &msg) {
  assert(m_initialised == true);
  printf("ErisLog: %s\n", msg.c_str());
}

int Client::connect(const std::string &host, int port) {
  assert(m_initialised == true);
  if (debug) printf("Client: Connect\n");

  if (m_status >= CLIENT_STATUS_CONNECTING) {
    m_system->pushMessage("Error: Connection already in progress", CONSOLE_MESSAGE);
    return 1;
  }
  // Check args
  if (host.empty()) {
    m_system->pushMessage("Error: No hostname specified", CONSOLE_MESSAGE);
    return 1;
  }

  if (debug) printf("Connecting to %s on port %d\n",  host.c_str(), port);

  assert(m_connection == NULL);

  // Create new eris connection object
  m_connection = new Eris::Connection(m_client_name, host, port, DEBUG_ERIS);

  // Set up connection callbacks
  m_connection->Failure.connect(SigC::slot(*this, &Client::NetFailure));
  m_connection->Connected.connect(SigC::slot(*this, &Client::NetConnected));
  m_connection->Disconnected.connect(SigC::slot(*this, &Client::NetDisconnected));
  m_connection->Disconnecting.connect(SigC::slot(*this, &Client::NetDisconnecting));
  m_connection->StatusChanged.connect(SigC::slot(*this, &Client::StatusChanged));
  
  m_system->pushMessage(CLIENT_CONNECTING, CONSOLE_MESSAGE);

  setStatus(CLIENT_STATUS_CONNECTING);
  if (m_connection->connect()  != 0) {
    printf("Error: Connection Error\n");
    m_system->pushMessage("Error: Connection Error", CONSOLE_MESSAGE);
    setStatus( CLIENT_STATUS_DISCONNECTED);
    delete m_connection;
    m_connection = NULL;
    return 1;
  }

  return 0;
}

int Client::disconnect() {
  assert ((m_initialised == true) && "Client not initialised");

  if (debug) printf("Client: disconnect\n");

  m_system->pushMessage(CLIENT_DISCONNECTING, CONSOLE_MESSAGE);

  if (m_status < CLIENT_STATUS_CONNECTED) {
    m_system->pushMessage("Error: Not connected", CONSOLE_MESSAGE);
    return 1;
  }

  assert(m_connection);
  assert(m_connection->isConnected());

  setStatus(CLIENT_STATUS_DISCONNECTING);
  if (m_connection->disconnect() != 0) {
    setStatus(CLIENT_STATUS_CONNECTED);
    printf("Error: Disconnect Error\n");
    m_system->pushMessage("Error: Disconnect Error", CONSOLE_MESSAGE);
    delete m_connection;
    m_connection = NULL;
    setStatus(CLIENT_STATUS_DISCONNECTED);
    return 1;
  }
  return 0;
}

int Client::createAccount(const std::string &username, const std::string &fullname, const std::string &password) {
  assert ((m_initialised == true) && "Client not initialised");

  if (m_status < CLIENT_STATUS_CONNECTED) {
    m_system->pushMessage("Error: Not Connected", CONSOLE_MESSAGE);
    return 1;
  }
  if (m_status == CLIENT_STATUS_LOGGING_IN) {
    m_system->pushMessage("Error: Already logging in", CONSOLE_MESSAGE);
    return 1;
  }
  if (m_status > CLIENT_STATUS_LOGGED_IN) {
    m_system->pushMessage("Error: Already logged in", CONSOLE_MESSAGE);
    return 1;
  }

  if (username.empty()) {
    m_system->pushMessage("Error: No username specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (fullname.empty()) {
    m_system->pushMessage("Error: No fullname specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (password.empty()) {
    m_system->pushMessage("Error: No password specified", CONSOLE_MESSAGE);
    return 1;
  }

  assert(m_connection);
  assert(m_connection->isConnected());

  if (debug) printf("Client::createAccount: Creating player\n");
  // m_account will still be around if previous attempts have failed
  if (m_account == NULL) {
    m_account = new Eris::Account(m_connection);
    // Setup callbacks
    m_account->LoginFailure.connect(SigC::slot(*this, &Client::LoginFailure));
    m_account->LoginSuccess.connect(SigC::slot(*this, &Client::LoginSuccess));
    m_account->LogoutComplete.connect(SigC::slot(*this, &Client::LogoutComplete));
    m_account->GotAllCharacters.connect(SigC::slot(*this, &Client::GotAllCharacters));
    m_account->GotCharacterInfo.connect(SigC::slot(*this, &Client::GotCharacterInfo));
    m_account->AvatarSuccess.connect(SigC::slot(*this, &Client::AvatarSuccess));
    m_account->AvatarFailure.connect(SigC::slot(*this, &Client::AvatarFailure));
  }
  try {
    setStatus(CLIENT_STATUS_LOGGING_IN);
    m_account->createAccount(username, fullname, password);
  } catch (Eris::InvalidOperation io) {
    printf("Eris Exception: %s\n", io._msg.c_str());
    setStatus(CLIENT_STATUS_CONNECTED);
    // Clean up object just to be safe
    delete m_account;
    m_account = NULL;
    return 1;
  } catch (...) {
    printf("Unknown Exception\n");
    setStatus(CLIENT_STATUS_CONNECTED);
    // Clean up object just to be safe
    delete m_account;
    m_account = NULL;
    return 1;
  }
  return 0;
}

int Client::login(const std::string &username, const std::string &password) {
  assert ((m_initialised == true) && "Client not initialised");

  if (debug) printf("Client: login\n");

  if (m_status != CLIENT_STATUS_CONNECTED) {
    if (m_status == CLIENT_STATUS_LOGGING_IN) {
      m_system->pushMessage("Error: Already logging in", CONSOLE_MESSAGE);
    }
    else if (m_status >= CLIENT_STATUS_LOGGED_IN) {
      m_system->pushMessage("Error: Already logged in", CONSOLE_MESSAGE);
    } else {
      m_system->pushMessage("Error: Not connected.", CONSOLE_MESSAGE);
    }
    return 1;
  }
  assert(m_connection != NULL);
  assert(m_connection->isConnected() == true);

  if (username.empty()) {
    m_system->pushMessage("Error: No username specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (password.empty()) {
    m_system->pushMessage("Error: No password specified", CONSOLE_MESSAGE);
    return 1;
  }

  if (debug) printf("Client::login: Creating player\n");
  
  if (m_account == NULL) {
    m_account = new Eris::Account(m_connection);
    // setup player callbacks
    m_account->LoginFailure.connect(SigC::slot(*this, &Client::LoginFailure));
    m_account->LoginSuccess.connect(SigC::slot(*this, &Client::LoginSuccess));
    m_account->LogoutComplete.connect(SigC::slot(*this, &Client::LogoutComplete));
    m_account->GotAllCharacters.connect(SigC::slot(*this, &Client::GotAllCharacters));
    m_account->GotCharacterInfo.connect(SigC::slot(*this, &Client::GotCharacterInfo));
    m_account->AvatarSuccess.connect(SigC::slot(*this, &Client::AvatarSuccess));
    m_account->AvatarFailure.connect(SigC::slot(*this, &Client::AvatarFailure));
  }
  try {
    setStatus(CLIENT_STATUS_LOGGING_IN);
    m_account->login(username, password);
  } catch (Eris::InvalidOperation io) {
    printf("Eris Exception: %s\n", io._msg.c_str());
    setStatus(CLIENT_STATUS_CONNECTED);
    // Clean up object just to be safe
    delete m_account;
    m_account = NULL;
    return 1;
  } catch(...) {
    printf("Unknown Exception\n");
    setStatus(CLIENT_STATUS_CONNECTED);
    // Clean up object just to be safe
    delete m_account;
    m_account = NULL;
    return 1;
  }
  return 0;
}

void Client::poll() {
  assert ((m_initialised == true) && "Client not initialised");
  try {
    Eris::PollDefault::poll();   
  } catch (Eris::InvalidOperation io) {
    printf("Eris Exception: %s\n", io._msg.c_str());
  } catch (Eris::BaseException be) {
    printf("Eris Exception: %s\n", be._msg.c_str());
  } catch (std::runtime_error re) {
    printf("STD::RUNTIME ERROR\n");
  } catch (...) {
    printf("Caught some error - ignoring\n");
  }
}

void Client::NetFailure(const std::string &msg)  {
  printf("Client::NetFailure\n");
  m_system->pushMessage("Network Failure: " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_DISCONNECTED);

  if (m_account) {
    delete m_account;
    m_account = NULL;
  }
  Eris::deleteLater(m_connection);
  m_connection = NULL;

}

void Client::NetConnected() {
  if (debug) printf("Client:NetConnected\n");
  m_system->pushMessage(CLIENT_CONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_CONNECTED);

}

void Client::NetDisconnected() {
  printf("Client::NetDisconnected\n");
  m_system->pushMessage(CLIENT_DISCONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_DISCONNECTED);

  if (m_account) {
    delete m_account;
    m_account = NULL;
  }
  Eris::deleteLater(m_connection);
  m_connection = NULL;
}

bool Client::NetDisconnecting() {
  if (debug) printf("Client::Disconnecting\n");
  m_system->pushMessage(CLIENT_DISCONNECTING,1);;
  return false;
}

void Client::LoginSuccess() {
  if (debug) printf("Client::LoginSuccess\n");
  m_system->pushMessage("Login Success", CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_LOGGED_IN);
}

int Client::createCharacter(const std::string &name, const std::string &type, const std::string &sex, const std::string &description) {
  assert ((m_initialised == true) && "Client not initialised");

  if (m_status != CLIENT_STATUS_LOGGED_IN) {
    if (m_status < CLIENT_STATUS_LOGGED_IN)  {
      m_system->pushMessage("Error: Not logged in", CONSOLE_MESSAGE);
    } else {
      m_system->pushMessage("Error: A character is already in use", CONSOLE_MESSAGE);
    }
    return 1;
  }

  assert(m_connection != NULL);
  assert(m_connection->isConnected() == true);
  assert(m_account != NULL);

  if (name.empty()) {
    m_system->pushMessage("Error: No character name specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (type.empty()) {
    m_system->pushMessage("Error: No character type specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (sex.empty()) {
    m_system->pushMessage("Error: No character gender specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (description.empty()) {
    m_system->pushMessage("Error: No character description specified", CONSOLE_MESSAGE);
    return 1;
  }


  if  (debug) printf("Client: Creating character - Name: %s Type: %s Sex: %s Description: %s\n", name.c_str(), type.c_str(), sex.c_str(), description.c_str());

  m_system->pushMessage("Creating Character: " +  name, CONSOLE_MESSAGE);

  // Create atlas character object
  Atlas::Objects::Entity::GameEntity ch;
  Atlas::Message::ListType prs(1, Atlas::Message::Element(type));
  ch->setParentsAsList(prs);
  ch->setName(name);
  ch->setAttr("sex", sex);
  ch->setAttr("description", description);

  try {
    setStatus(CLIENT_STATUS_GOING_IN_WORLD);
    m_account->createCharacter(ch);
  } catch (Eris::InvalidOperation io) {
    setStatus(CLIENT_STATUS_LOGGED_IN);
    m_system->pushMessage(io._msg, CONSOLE_MESSAGE);
    printf("Eris Exception: %s\n", io._msg.c_str());
    return 1;
  } catch (...){
    setStatus( CLIENT_STATUS_LOGGED_IN);
    m_system->pushMessage("Error creating character", CONSOLE_MESSAGE);
    printf("Unkown Exception\n");
    return 1;
  }
  if (debug) printf("Client::createCharacter: Setting up callbacks\n");

  return 0;
}

int Client::takeCharacter(const std::string &id) {
  assert ((m_initialised == true) && "Client not initialised");
  if (debug) printf("Client::takeCharacter\n");
  if (m_status != CLIENT_STATUS_LOGGED_IN) {
     return 1;
  }

  assert(m_connection != NULL);
  assert(m_connection->isConnected() == true);
  assert (m_account != NULL);

  if (id.empty()) {
    m_takeFirst = true;
    getCharacters();
    return 0;
  }

  if (debug) printf("Client::takeCharacter: Taking character - %s\n", id.c_str());
  m_system->pushMessage(std::string(CLIENT_TAKE_CHARACTER) + std::string(": ") + id, CONSOLE_MESSAGE);
  
  try {
    setStatus(CLIENT_STATUS_GOING_IN_WORLD);
    m_account->takeCharacter(id);
  } catch (Eris::InvalidOperation io) {
    setStatus(CLIENT_STATUS_LOGGED_IN);
    printf("Eris Exception: %s\n", io._msg.c_str());
    return 1;
  } catch (...) {
    setStatus(CLIENT_STATUS_LOGGED_IN);
    printf("Unkown Exception\n");
    return 1;
  }

  return 0;
}

int Client::logout() {
  assert ((m_initialised == true) && "Client not initialised");
  if (debug) printf("Client::logout\n");

  if (m_account == NULL) {
    m_system->pushMessage("Error: Not logged in", CONSOLE_MESSAGE);
    return 1;
  }

  if (m_status >= CLIENT_STATUS_LOGGED_IN) {
    m_system->pushMessage(CLIENT_LOGGING_OUT,CONSOLE_MESSAGE);
    printf("Client::logout: Logging Out\n");
    setStatus(CLIENT_STATUS_LOGGING_OUT);
    m_account->logout();
  }
  return 0;
}

int Client::getCharacters() {
  assert ((m_initialised == true) && "Client not initialised");
  if (m_account) {
    m_account->refreshCharacterInfo();
  } else {
    m_system->pushMessage("Error: Not logged in", CONSOLE_MESSAGE);
  }
  return 0;
}

std::string Client::getStatus() {
  assert ((m_initialised == true) && "Client not initialised");
  printf("Client::getStatus\n");
  if (m_connection == NULL) return "No Connection object";
  switch(m_connection->getStatus()) {
    case 0: return "INVALID_STATUS";
    case 1: return "NEGOTIATE";
    case 2: return "CONNECTING";
    case 3: return "CONNECTED";
    case 4: return "DISCONNECTED";
    case 5: return "DISCONNECTING";
    default: return "OTHER";
  }
}

void Client::StatusChanged(Eris::Connection::Status s) {
  if (debug) printf("Status Changed: %s\n", getStatus().c_str());
}

void Client::LoginFailure(const std::string& msg) {
  printf("Login Failure: %s\n", msg.c_str());
  m_system->pushMessage("Login Failure" + msg, CONSOLE_MESSAGE);
  setStatus(CLIENT_STATUS_CONNECTED);
}

void Client::LogoutComplete(bool clean_logout) {
  if (clean_logout) {
    //Logged out cleanly
    printf("Clean Logout\n");
  }  else {
    // Emitted due to timeout or other error
    printf("Error during Logout\n");
  }
  m_system->pushMessage(CLIENT_LOGGED_OUT, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_CONNECTED);

  Eris::deleteLater(m_account);
  m_account = NULL;
}

void Client::GotCharacterInfo(const Atlas::Objects::Entity::GameEntity& ge) {
  if (debug) printf("Got Char - Name: %s ID: %s\n ", ge->getName().c_str(), ge->getId().c_str());
}

void Client::GotAllCharacters() {
  if (debug) printf("Client::getCharacters\n");
  assert(m_account);

  Eris::CharacterMap m = m_account->getCharacters();
  for (Eris::CharacterMap::const_iterator I = m.begin(); I != m.end(); ++I) {
    m_system->pushMessage(I->first.c_str(), CONSOLE_MESSAGE);
    if (m_takeFirst) {
      takeCharacter(I->first.c_str());
      m_takeFirst = false;
    }
  }
}

void Client::registerCommands(Console *console) {
  assert ((m_initialised == true) && "Client not initialised");
  console->registerCommand(SERVER_CONNECT, this);
  console->registerCommand(SERVER_DISCONNECT, this);
  console->registerCommand(ACCOUNT_CREATE, this);
  console->registerCommand(ACCOUNT_LOGIN, this);
  console->registerCommand(ACCOUNT_LOGOUT, this);
  console->registerCommand(CHARACTER_LIST, this);
  console->registerCommand(CHARACTER_CREATE, this);
  console->registerCommand(CHARACTER_TAKE, this);
  console->registerCommand(GET_SERVERS, this);
  console->registerCommand(STOP_SERVERS, this);
}

void Client::runCommand(const std::string &command, const std::string &args) {
  assert ((m_initialised == true) && "Client not initialised");
  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(args);
  if (command == SERVER_CONNECT) {
    std::string server = tokeniser.nextToken();
    std::string port_string = tokeniser.nextToken();
    if (port_string.empty()) {
      connect(server, DEFAULT_PORT);
    } else {
      int port = 0;
      cast_stream(port_string, port);
      connect(server, port);
    }
  }
  else if (command == SERVER_DISCONNECT) {
    disconnect();
  }
  else if (command == ACCOUNT_CREATE) {
    std::string user_name = tokeniser.nextToken();
    std::string password = tokeniser.nextToken();
    std::string full_name = tokeniser.remainingTokens();
    createAccount(user_name, full_name, password);
  }
  else if (command == ACCOUNT_LOGIN) {
    std::string user_name = tokeniser.nextToken();
    std::string password = tokeniser.nextToken();
    login(user_name, password);
  }
  else if (command == ACCOUNT_LOGOUT) {
    logout();
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
}

void Client::AvatarSuccess(Eris::Avatar *avatar) {
  assert(avatar != NULL);
  printf("Avatar sucessfully created\n");
  m_avatar = avatar;

  Factory* f = new Factory(*m_connection->getTypeService());
  m_avatar->getView()->registerFactory(f);

  m_avatar->GotCharacterEntity.connect(SigC::slot(*this, &Client::GotCharacterEntity));
}

void Client::AvatarFailure(const std::string &msg) {
  std::cerr << "AvatarFailure: " << msg << std::endl;
  m_system->pushMessage(msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  m_status = CLIENT_STATUS_LOGGED_IN;
  m_system->getCharacter()->setAvatar(NULL);
}

void Client::GotCharacterEntity(Eris::Entity *e) {
  assert(e != NULL);
  assert(m_avatar != NULL);

  m_system->getCharacter()->setAvatar(m_avatar);
  setStatus(CLIENT_STATUS_IN_WORLD);
}

void Client::setStatus(int status) {
  assert(m_system);
  assert(m_system->getCharacter());
  switch (status) {
    case CLIENT_STATUS_DISCONNECTED:
    case CLIENT_STATUS_DISCONNECTING:
    case CLIENT_STATUS_CONNECTING:
      m_system->setState(SYS_CONNECTED, false);
      m_system->setState(SYS_LOGGED_IN, false);
      m_system->setState(SYS_IN_WORLD, false);
      m_system->getCharacter()->setAvatar(NULL);
      break;
    case CLIENT_STATUS_CONNECTED:
    case CLIENT_STATUS_LOGGING_IN:
    case CLIENT_STATUS_LOGGING_OUT:
      m_system->setState(SYS_CONNECTED, true);
      m_system->setState(SYS_LOGGED_IN, false);
      m_system->setState(SYS_IN_WORLD, false);
      m_system->getCharacter()->setAvatar(NULL);
      break;
    case CLIENT_STATUS_LOGGED_IN:
    case CLIENT_STATUS_GOING_IN_WORLD:
    case CLIENT_STATUS_GOING_OUT_WORLD:
      m_system->setState(SYS_CONNECTED, true);
      m_system->setState(SYS_LOGGED_IN, true);
      m_system->setState(SYS_IN_WORLD, false);
      m_system->getCharacter()->setAvatar(NULL);
      break;
    case CLIENT_STATUS_IN_WORLD:
      m_system->setState(SYS_CONNECTED, true);
      m_system->setState(SYS_LOGGED_IN, true);
      m_system->setState(SYS_IN_WORLD, true);
      break;
  }
  m_status = status;
}

} /* namespace Sear */
