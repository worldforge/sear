// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: client.cpp,v 1.61 2005-02-18 16:39:06 simon Exp $
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "System.h"

#include <Atlas/Objects/Entity.h>

#include <Eris/Connection.h>
#include <Eris/Avatar.h>
#include <Eris/Log.h>
//#include <Eris/Lobby.h>
#include <Eris/Entity.h>
#include <Eris/Person.h>
#include <Eris/PollDefault.h>
#include <Eris/Metaserver.h>
#include <Eris/ServerInfo.h>
#include <Eris/Exceptions.h>
#include <sigc++/object.h>
#include <sigc++/slot.h>
#include <sigc++/object_slot.h>
#include <sigc++/bind.h>

#include "common/Log.h"
#include "common/Utility.h"

#include "ActionHandler.h"
#include "client.h"
#include "Console.h"
#include "Factory.h"
#include "Character.h"
//#include "EventHandler.h"
//#include "Event.h"
//#include "Lobby.h"
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

// Console Command strings
static std::string SERVER_CONNECT = "connect";
static std::string SERVER_RECONNECT = "reconnect";
static std::string SERVER_DISCONNECT = "disconnect";
static std::string ACCOUNT_CREATE = "create";
static std::string ACCOUNT_LOGIN = "login";
static std::string ACCOUNT_LOGOUT = "logout";
static std::string CHARACTER_LIST = "get";
static std::string CHARACTER_CREATE = "add";
static std::string CHARACTER_TAKE = "take";
static std::string GET_SERVERS = "get_servers";
static std::string STOP_SERVERS = "stop_servers";


/*
 * The Constructor. Creates the initial Eris Connection object and
 * 
 * @param client_name - Name of client to pass to Eris
*/
Client::Client(System *system, const std::string &client_name) :
  _system(system),
  m_connection(NULL),
  m_account(NULL),
  m_avatar(NULL),
//  _lobby(NULL),
//  the_lobby(NULL),
//  _meta(NULL),
  _factory(NULL),
  _status(CLIENT_STATUS_DISCONNECTED),
  _client_name(client_name),
  _initialised(false),
  m_takeFirst(false)
{
  assert((system != NULL) && "System is NULL");
}

Client::~Client() {
  if (_initialised) shutdown();
}

bool Client::init() {
  if (_initialised) shutdown();

  // Setup logging
  Eris::setLogLevel((Eris::LogLevel) ERIS_LOG_LEVEL);
  Eris::Logged.connect(SigC::slot(*this, &Client::Log));

  // Create new eris connection object
  m_connection = new Eris::Connection(_client_name, DEBUG_ERIS);
//  if (!m_connection) {
//    Log::writeLog("Client - Connection is NULL", Log::LOG_ERROR);
//    throw ClientException("Connection is NULL", ClientException::BAD_CONNECTION);
//  }
  // Set up connection callbacks
  m_connection->Failure.connect(SigC::slot(*this, &Client::NetFailure));
  m_connection->Connected.connect(SigC::slot(*this, &Client::NetConnected));
  m_connection->Disconnected.connect(SigC::slot(*this, &Client::NetDisconnected));
  m_connection->Disconnecting.connect(SigC::slot(*this, &Client::NetDisconnecting));
//  m_connection->Timeout.connect(SigC::slot(*this, &Client::Timeout));
  m_connection->StatusChanged.connect(SigC::slot(*this, &Client::StatusChanged));
  
  _initialised = true;
  return true;
}

void Client::shutdown() {
//  if (the_lobby) {
//    delete the_lobby;
//    the_lobby = NULL;
//  }
  if (m_account) {
    delete m_account;
    m_account = NULL;
  }
  if (m_connection) {
    delete m_connection;
    m_connection = NULL;
  }
  if (_factory) {
    delete _factory;
    _factory = NULL;
  }
//  if (_meta) {
//    delete _meta;
//    _meta = NULL;
//  }
  _initialised = false;
}

void Client::Log(Eris::LogLevel lvl, const std::string &msg) {
  Log::writeLog(msg, Log::LOG_ERIS);
}

int Client::connect(const std::string &host, int port) {
  assert ((_initialised == true) && "Client not initialised");
  Log::writeLog(std::string("Connecting to ") + host + std::string(" on port ") + string_fmt(port) , Log::LOG_DEFAULT);
//  if (!m_connection) throw ClientException("Connection object is NULL", ClientException::NO_CONNECTION_OBJECT);
  if (m_connection->isConnected()) throw ClientException("Already Connected", ClientException::ALREADY_CONNECTED);
  if (host.empty()) throw ClientException("No hostname given", ClientException::NO_HOSTNAME);
  if (debug) Log::writeLog("Calling Connect", Log::LOG_INFO);
  _system->pushMessage(CLIENT_CONNECTING, CONSOLE_MESSAGE);
  try {
    m_connection->connect(host, port);
  } catch (Eris::InvalidOperation io) {
    if (io._msg == "existing connection instance in Connection ctor") {
      Log::writeLog("Client::connect: Caught InvalidOperation: " + io._msg, Log::LOG_INFO);
      throw ClientException("Error connecting", ClientException::ALREADY_CONNECTED);
    } else {
      Log::writeLog("Client::connect: Caught InvalidOperation: " + io._msg, Log::LOG_INFO);
      throw ClientException("Error connecting", ClientException::ERROR_CONNECTING);
    }
  } catch (...) {
    Log::writeLog("Client::connect: Caught Unknown Exception", Log::LOG_INFO);
    throw ClientException("Error connecting", ClientException::ERROR_CONNECTING);
  }
  return 0;
}

int Client::reconnect() {
  assert ((_initialised == true) && "Client not initialised");
  if (debug) Log::writeLog("Client::reconnect", Log::LOG_INFO);
  _system->pushMessage(CLIENT_RECONNECTING, CONSOLE_MESSAGE);
//  if (m_connection == NULL) throw ClientException("Connection object is NULL", ClientException::NO_CONNECTION_OBJECT);
  if (!m_connection->isConnected()) throw ClientException("Not Connected", ClientException::NOT_CONNECTED);
  try {
    m_connection->reconnect();
  } catch (Eris::InvalidOperation io) {
    if (io._msg == "existing connection instance in Connection ctor") {
      Log::writeLog("Client::reconnect: Caught InvalidOperation: " + io._msg, Log::LOG_INFO);
      throw ClientException("Error reconnecting", ClientException::ALREADY_CONNECTED);
    } else {
      Log::writeLog("Client::reconnect: Caught InvalidOperation: " + io._msg, Log::LOG_INFO);
      throw ClientException("Error reconnecting", ClientException::ERROR_CONNECTING);
    }
  } catch (...) {
    Log::writeLog("Client::reconnect: Caught Unknown Exception", Log::LOG_INFO);
    throw ClientException("Error reconnecting", ClientException::ERROR_CONNECTING);
  }
  return 0;
}

int Client::disconnect() {
  assert ((_initialised == true) && "Client not initialised");
  if (debug) Log::writeLog("Client::disconnect", Log::LOG_INFO);
  _system->pushMessage(CLIENT_DISCONNECTING, CONSOLE_MESSAGE);
//  if (m_connection == NULL) throw ClientException("Connection object is NULL", ClientException::NO_CONNECTION_OBJECT);
  if (!m_connection->isConnected()) throw ClientException("Not Connected", ClientException::NOT_CONNECTED);
  //Are we logged in?
  if (m_account != NULL) {
    Log::writeLog("Client::disconnect: Logging out first...", Log::LOG_INFO);
//    logout();
  }
  try {
    m_connection->disconnect();
  } catch (Eris::InvalidOperation io) {
    if (io._msg == "Bad connection state for disconnection") {
      Log::writeLog("Client::disconnect: Caught InvalidOperation: " + io._msg, Log::LOG_INFO);
      throw ClientException("Bad disconnection state", ClientException::BAD_STATE);
    } else {
      Log::writeLog("Client::disconnect: Caught InvalidOperation: " + io._msg, Log::LOG_INFO);
      throw ClientException("Error disconnecting", ClientException::ERROR_DISCONNECTING);
    }
  } catch (...) {
    Log::writeLog("Client::disconnect: Caught Unknown Exception", Log::LOG_INFO);
    throw ClientException("Error disconnecting", ClientException::ERROR_DISCONNECTING);
  }
  return 0;
}

int Client::createAccount(const std::string &username, const std::string &fullname, const std::string &password) {
  assert ((_initialised == true) && "Client not initialised");
  if (_status != CLIENT_STATUS_CONNECTED) throw ClientException("Not Connected", ClientException::BAD_CONNECTION);
//  if (m_connection == NULL) throw ClientException("No connection object!", ClientException::BAD_CONNECTION);
  if (!m_connection->isConnected()) throw ClientException("Not Connected!", ClientException::BAD_CONNECTION);
  if (username.empty()) throw ClientException("No Username!", ClientException::BAD_USERNAME);
  if (fullname.empty()) throw ClientException("No Fullname!", ClientException::BAD_FULLANME);
  if (password.empty()) throw ClientException("No Password!", ClientException::BAD_PASSWORD);

  //Have we already tried logging in?
  if (debug) Log::writeLog("Client::createAccount: Creating player", Log::LOG_INFO);
  if (m_account == NULL) {
    m_account = new Eris::Account(m_connection);
    // Setup player callbacks
    m_account->LoginFailure.connect(SigC::slot(*this, &Client::LoginFailure));
    m_account->LoginSuccess.connect(SigC::slot(*this, &Client::LoginSuccess));
    m_account->LogoutComplete.connect(SigC::slot(*this, &Client::LogoutComplete));
    m_account->GotAllCharacters.connect(SigC::slot(*this, &Client::GotAllCharacters));
    m_account->GotCharacterInfo.connect(SigC::slot(*this, &Client::GotCharacterInfo));
    m_account->AvatarSuccess.connect(SigC::slot(*this, &Client::AvatarSuccess));
    m_account->AvatarFailure.connect(SigC::slot(*this, &Client::AvatarFailure));
  }
  try {
    m_account->createAccount(username, fullname, password);
  } catch (Eris::InvalidOperation e) {
    if (e._msg == "Invalid connection") {
      Log::writeLog("Error with connection", Log::LOG_INFO);
      throw ClientException("Error with connection", ClientException::BAD_CONNECTION);
    } else if (e._msg.substr(0,19) == "action in progress") {
      Log::writeLog("Cannot create account, action already in progress", Log::LOG_INFO);
      throw ClientException("Error with connection", ClientException::BAD_CONNECTION);
    }
    //Reset variables used here
    Log::writeLog("Client::createAccount: Exception caught", Log::LOG_INFO);
    Log::writeLog("Invalid Operation: " + e._msg, Log::LOG_ERROR);
    throw Exception("Error creating account");
  }
  return 0;
}

int Client::login(const std::string &username, const std::string &password) {
  assert ((_initialised == true) && "Client not initialised");
  if (debug) Log::writeLog("Client::login", Log::LOG_INFO);
  if (_status < CLIENT_STATUS_CONNECTED) throw ClientException("Not Connected", ClientException::NOT_CONNECTED);
  if (_status == CLIENT_STATUS_LOGGED_IN) throw Exception("Already Logged In");
//  if (m_connection == NULL) throw ClientException("No Connection Object", ClientException::NO_CONNECTION_OBJECT);
  if (!m_connection->isConnected()) throw Exception("Not Connected");
  if (username.empty()) throw Exception("No Username");
  if (password.empty()) throw Exception("No Password");
//  std::string msg = std::string(CLIENT_LOGGING_IN) + std::string(": ") + username; 
//  _system->pushMessage(msg);
  if (debug) Log::writeLog("Client::login: Creating player", Log::LOG_INFO);
  
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
    m_account->login(username, password);
  } catch (Eris::InvalidOperation e) {
    Log::writeLog("Client::login: Caught Exception - Login Failed", Log::LOG_INFO);
    Log::writeLog("Invalid Operation: " + e._msg, Log::LOG_ERROR);
    throw Exception("ERIS Bad Login");
  } catch(...) {
    Log::writeLog("Client::login: Caught Exception", Log::LOG_INFO);
    Log::writeLog("Other Error", Log::LOG_ERROR);
    throw Exception("Bad Login");
  }
  return 0;
}

void Client::poll() {
  assert ((_initialised == true) && "Client not initialised");
  try {
//    if (m_connection != NULL) m_connection->poll();
    Eris::PollDefault::poll();   
  } catch (Eris::InvalidOperation io) {
    Log::writeLog(io._msg, Log::LOG_ERROR);
  } catch (Eris::BaseException be) {
    Log::writeLog(be._msg, Log::LOG_ERROR);
  } catch (std::runtime_error re) {
    Log::writeLog("STD::RUNTIME ERROR", Log::LOG_ERROR);
  } catch (...) {
    Log::writeLog("Caught some error - ignoring", Log::LOG_ERROR);
  }
}

void Client::NetFailure(const std::string &msg)  {
  Log::writeLog("Client::NetFailure", Log::LOG_INFO);
  _system->pushMessage("Network Failure: " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_DISCONNECTED;
  _system->setState(SYS_CONNECTED, false);
  _system->setState(SYS_LOGGED_IN, false);
  _system->setState(SYS_IN_WORLD, false);
}

void Client::NetConnected() {
  if (debug) Log::writeLog("Client:NetConnected", Log::LOG_INFO);
  _system->pushMessage(CLIENT_CONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_CONNECTED;
//  try {
//    _lobby = Eris::Lobby::instance();
//    _lobby->LoggedIn.connect(SigC::slot(*this, &Client::LoggedIn));
//    _lobby->SightPerson.connect(SigC::slot(*this, &Client::SightPerson));
//    if (the_lobby) {
//      delete the_lobby;
//    }
//    the_lobby = new Lobby();
//    the_lobby->init(_lobby);
//    the_lobby->registerCommands(System::instance()->getConsole());
//  } catch (Eris::InvalidOperation io) {
//    Log::writeLog("Invalid Operation: "  + io._msg, Log::LOG_ERROR);
//  }
  _system->setState(SYS_CONNECTED, true);
}

void Client::NetDisconnected() {
  Log::writeLog("Client::NetDisconnected", Log::LOG_INFO);
  _system->pushMessage(CLIENT_DISCONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_DISCONNECTED;
  _system->setState(SYS_CONNECTED, false);
}

bool Client::NetDisconnecting() {
  Log::writeLog("Client::Disconnecting", Log::LOG_INFO);
  _system->pushMessage(CLIENT_DISCONNECTING,1);;
  return false;
}

//void Client::LoggedIn(const Atlas::Objects::Entity::Account &a) {
//  if (debug) Log::writeLog("Client::LoggedIn", Log::LOG_INFO);
//  _system->pushMessage(CLIENT_LOGGED_IN, CONSOLE_MESSAGE | SCREEN_MESSAGE);
//  _status = CLIENT_STATUS_LOGGED_IN;
////  _system->setState(SYS_LOGGED_IN, true);
//}

void Client::LoginSuccess() {
  if (debug) Log::writeLog("Client::LoginSuccess", Log::LOG_INFO);
  _system->pushMessage("Login Success", CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_LOGGED_IN;
//  _system->setState(SYS_LOGGED_IN, true);
}

int Client::createCharacter(const std::string &name, const std::string &type, const std::string &sex, const std::string &description) {
  assert ((_initialised == true) && "Client not initialised");
  // Check conditions
  if (_status != CLIENT_STATUS_LOGGED_IN) throw Exception("Not logged in");
//  if (m_connection == NULL) throw Exception("Connection is NULL");
  if (!m_connection->isConnected()) throw Exception("Not connected");
  if (name.empty()) throw Exception("No name given");
  if (description.empty()) throw Exception ("no description given");
  if (m_account == NULL) throw Exception("Account is NULL");
//  if (_lobby == NULL) throw Exception ("Lobby is NULL");
  Log::writeLog("Client: Creating character - Name: " + name + " Type: " + type + " Sex: " + sex + " Description: " + description, Log::LOG_DEFAULT);
  _system->pushMessage("Creating Character: " +  name, CONSOLE_MESSAGE);
  Atlas::Objects::Entity::GameEntity ch;
  Atlas::Message::Element::ListType prs(1, Atlas::Message::Element(type));
  ch->setParentsAsList(prs);
  ch->setName(name);
  ch->setAttr("sex", sex);
  ch->setAttr("description", description);
  try {
    m_account->createCharacter(ch);
  } catch (Eris::InvalidOperation ie) {
    Log::writeLog("Client::createCharacter: Creation failed", Log::LOG_INFO);
    _system->pushMessage("Character Creation Failed: " +  ie._msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
   throw Exception ("ERIS Bad creation");
  } catch (...){
    Log::writeLog("Client::createCharacter: Caught Exception", Log::LOG_INFO);
    _system->pushMessage("Unknown Create Character Error", CONSOLE_MESSAGE | SCREEN_MESSAGE);
    throw Exception("Bad creation");
  }
  Log::writeLog("Client::createCharacter: Setting up callbacks", Log::LOG_INFO);
//  world->GotTime.connect(SigC::slot(*this, &Client::GotTime));
//  world->Entered.connect(SigC::slot(*this, &Client::Entered));
//  world->EntityCreate.connect(SigC::slot(*this, &Client::EntityCreate));
//  world->EntityDelete.connect(SigC::slot(*this, &Client::EntityDelete));
//  world->Appearance.connect(SigC::slot(*this, &Client::Appearance));
//  world->Disappearance.connect(SigC::slot(*this, &Client::Disappearance));
  if (_factory) delete _factory;
  _factory = new Factory(*m_connection->getTypeService());
  Eris::Factory::registerFactory(_factory);
//  world->registerFactory(_factory);
  return 0;
}

int Client::takeCharacter(const std::string &id) {
  assert ((_initialised == true) && "Client not initialised");
  if (debug) Log::writeLog("Client::takeCharacter", Log::LOG_INFO);
  if (_status != CLIENT_STATUS_LOGGED_IN) throw Exception("not logged in");
//  if (m_connection == NULL) throw Exception("Connection is NULL");
  if (!m_connection->isConnected()) throw Exception("not connected");
  if (m_account == NULL) throw Exception("Account is NULL");
//  if (_lobby == NULL) throw Exception("lobby is NULL");
  if (id.empty()) {
    m_takeFirst = true;
    getCharacters();
    return 0;
  }

  Log::writeLog("Client::takeCharacter: Taking character - " + id, Log::LOG_INFO);
  _system->pushMessage(std::string(CLIENT_TAKE_CHARACTER) + std::string(": ") + id, CONSOLE_MESSAGE);
  
  try {
    m_account->takeCharacter(id);
  } catch (Eris::InvalidOperation ie) {
    Log::writeLog("Client::takeCharacter: Error", Log::LOG_INFO);
    _system->pushMessage("Error Taking Character: " + ie._msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
    throw Exception("ERIS Bad take");
  } catch (...) {
    Log::writeLog("Client::takeCharacter: Unspecified Error", Log::LOG_INFO);
    _system->pushMessage("Unknown Take Character Error", CONSOLE_MESSAGE | SCREEN_MESSAGE);
    throw Exception ("Bad take");
  }
  Log::writeLog("Client::takeCharacter: Setting up callbacks", Log::LOG_INFO);
//  world->GotTime.connect(SigC::slot(*this, &Client::GotTime));
//  world->Entered.connect(SigC::slot(*this, &Client::Entered));  
//  world->EntityCreate.connect(SigC::slot(*this, &Client::EntityCreate));
//  world->EntityDelete.connect(SigC::slot(*this, &Client::EntityDelete));
//  world->Appearance.connect(SigC::slot(*this, &Client::Appearance));
//  world->Disappearance.connect(SigC::slot(*this, &Client::Disappearance));
  if (_factory) delete _factory;
  _factory = new Factory(*m_connection->getTypeService());
  Eris::Factory::registerFactory(_factory);
  return 0;
}


//void Client::SightPerson(Eris::Person *person) {
//  if (debug) Log::writeLog("Client::SightPerson: " + person->getName(), Log::LOG_INFO);
//  _system->pushMessage(std::string(CLIENT_SIGHTED_PERSON) + " " + person->getName(), CONSOLE_MESSAGE);
////  Log::writeLog(person->getName());
////  Log::writeLog(person->getAccount());
//  // This is here due to bugs in ERIS ??,
//  _system->setState(SYS_LOGGED_IN, true);
//}

int Client::logout() {
  assert ((_initialised == true) && "Client not initialised");
  if (debug) Log::writeLog("Client::logout", Log::LOG_INFO);
//  if (m_connection == NULL) throw Exception("Connection is NULL");
  if (!m_connection->isConnected()) throw Exception("Not Connected");
  if (m_account == NULL) throw Exception("Account is NULL");
  if (_status == CLIENT_STATUS_LOGGED_IN) {
    _system->pushMessage(CLIENT_LOGGING_OUT,CONSOLE_MESSAGE);
    Log::writeLog("Client::logout: Logging Out", Log::LOG_INFO);
    m_account->logout();
  }
  return 0;
}

int Client::getCharacters() {
  assert ((_initialised == true) && "Client not initialised");
  if (m_account) m_account->refreshCharacterInfo();
  return 0;
}

std::string Client::getStatus() {
  assert ((_initialised == true) && "Client not initialised");
  Log::writeLog("Client::getStatus", Log::LOG_INFO);
//  if (m_connection == NULL) return "NO OBJECT";
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

//int Client::listRooms() {
//  assert ((_initialised == true) && "Client not initialised");
//  if (debug) Log::writeLog("Client::listRooms", Log::LOG_INFO);
////  if (m_connection == NULL) throw Exception("Connection is NULL");
//  if (!m_connection->isConnected()) throw Exception("Not Conncted");
//  if (m_account == NULL) throw Exception("Account is NULL");
////  if (_lobby == NULL) throw Exception("Lobby is NULL");
  //_system->pushMessage("Room Listings:", CONSOLE_MESSAGE);
//  Log::writeLog("Client::listRooms: Start of list", Log::LOG_INFO);
//  Eris::StringList i = _lobby->getRooms();
//  for (Eris::StringList::iterator I=i.begin(); I != i.end(); ++I) {
//    _system->pushMessage(I->c_str(), CONSOLE_MESSAGE);
//  }
//  Log::writeLog("End of List", Log::LOG_INFO);
//  return 0;
//}

//void Client::EntityCreate(Eris::Entity* e) {
//  if (debug) Log::writeLog("Entity Created: " + e->getName(), Log::LOG_INFO);
//  // TODO - Is this needed?
//  ((WorldEntity *)e)->handleMove();
//}

//void Client::EntityDelete(Eris::Entity* e){
//  if (debug) Log::writeLog("Entity Deleted: " + e->getName(), Log::LOG_INFO);
//}

//void Client::Entered(Eris::Entity* e){
//  Log::writeLog("Entered World", Log::LOG_INFO);
//  _status = CLIENT_STATUS_IN_WORLD;
//  _system->getEventHandler()->addEvent(Event(EF_HANDLE_MOVE, e, EC_IN_WORLD, 0));
//  _system->getActionHandler()->handleAction("enter_world", NULL);
//  
//}

//void Client::Appearance(Eris::Entity *e){
//if (debug)  Log::writeLog(std::string("Appearance: ") + e->getName(), Log::LOG_INFO);
//  if (!e->hasAttr("guise")) return;
///
//  Atlas::Message::Element::MapType mt = e->valueOfAttr("guise").asMap();    ObjectRecord *record = NULL;
//  ObjectHandler *object_handler = System::instance()->getObjectHandler();
//  if (object_handler) record = object_handler->getObjectRecord(e->getId());
//  if (record) record->setAppearance(mt);
  
//}

//void Client::Disappearance(Eris::Entity *e){
//  if (debug) Log::writeLog("Disappearance: " + e->getName(), Log::LOG_INFO);
//}

//void Client::RootEntityChanged(Eris::Entity *e) {
//  Log::writeLog("RootEntityChanged: " +  e->getId(), Log::LOG_INFO);
//}

//void Client::Timeout(Eris::Connection::Status s) {
//  _system->pushMessage("Error: Timeout", 3);
//  Log::writeLog(CLIENT_TIMEOUT, Log::LOG_INFO);
//}

void Client::StatusChanged(Eris::Connection::Status s) {
  if (debug) Log::writeLog(CLIENT_STATUS_CHANGED, Log::LOG_INFO);
}

void Client::LoginFailure(const std::string& msg) {
  Log::writeLog("Login Failure" + msg, Log::LOG_DEFAULT);
  _system->pushMessage("Login Failure" + msg, CONSOLE_MESSAGE);
}

void Client::LogoutComplete(bool clean_logout) {
//  delete m_account;
  if (clean_logout) {
    Log::writeLog("Clean Logout", Log::LOG_DEFAULT);
    //Logged out cleanly
  }  else {
    Log::writeLog("Error during Logout", Log::LOG_ERROR);
    // Emitted due to timeout or other error
  }
  _system->pushMessage(CLIENT_LOGGED_OUT, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_CONNECTED;
  _system->setState(SYS_LOGGED_IN, false);
}

void Client::GotCharacterInfo(const Atlas::Objects::Entity::GameEntity& ge) {
  Log::writeLog("Got Char - Name: " + ge->getName() + " ID: " + ge->getId(), Log::LOG_INFO);
}

void Client::GotAllCharacters() {
  if (debug) Log::writeLog("Client::getCharacters", Log::LOG_INFO);
//  if (m_connection == NULL) throw Exception("Connection is NULL");
//  if (!m_connection->isConnected()) throw Exception("Not Connected");
  if (m_account == NULL) throw Exception ("Account is NULL");
//  if (_lobby == NULL) throw Exception("Lobby is NULL");
  Eris::CharacterMap m = m_account->getCharacters();
  for (Eris::CharacterMap::const_iterator I = m.begin(); I != m.end(); ++I) {
//    Atlas::Objects::Entity::GameEntity ge = *I->first;
    _system->pushMessage(I->first.c_str(), CONSOLE_MESSAGE);
    if (m_takeFirst) {
      takeCharacter(I->first.c_str());
      m_takeFirst = false;
    }
  }
}

void Client::registerCommands(Console *console) {
  assert ((_initialised == true) && "Client not initialised");
  //TODO: we could register the commands as they become available?
  console->registerCommand(SERVER_CONNECT, this);
  console->registerCommand(SERVER_RECONNECT, this);
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
  assert ((_initialised == true) && "Client not initialised");
  Tokeniser tokeniser = Tokeniser();
  tokeniser.initTokens(args);
  if (command == SERVER_CONNECT) {
    std::string server = tokeniser.nextToken();
    std::string port_string = tokeniser.nextToken();
    try {
      if (port_string.empty()) {
        connect(server, DEFAULT_PORT);
      } else {
        int port = 0;
        cast_stream(port_string, port);
        connect(server, port);
      }
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == SERVER_RECONNECT) {
    try {
      reconnect();
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == SERVER_DISCONNECT) {
    try {
      disconnect();
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == ACCOUNT_CREATE) {
    std::string user_name = tokeniser.nextToken();
    std::string password = tokeniser.nextToken();
    std::string full_name = tokeniser.remainingTokens();
    try {
      createAccount(user_name, full_name, password);
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == ACCOUNT_LOGIN) {
    std::string user_name = tokeniser.nextToken();
    std::string password = tokeniser.nextToken();
    try {
      login(user_name, password);
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == ACCOUNT_LOGOUT) {
    try {
      logout();
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == CHARACTER_LIST) { 
    try {
      getCharacters();
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == CHARACTER_CREATE) {
    std::string name = tokeniser.nextToken();
    std::string type = tokeniser.nextToken();
    std::string sex = tokeniser.nextToken();
    std::string desc = tokeniser.remainingTokens();
    try {
      createCharacter(name, type, sex, desc);
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  }
  else if (command == CHARACTER_TAKE) {
    try {
      takeCharacter(args);
    } catch (ClientException ce) {
      Log::writeLog(ce.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(ce.getMessage(), CONSOLE_MESSAGE);
    } catch (Exception e) {
      Log::writeLog(e.getMessage(), Log::LOG_ERROR);
      _system->pushMessage(e.getMessage(), CONSOLE_MESSAGE);
    }
  } 
//  else if (command == GET_SERVERS) getServers();
//  else if (command == STOP_SERVERS) stopServers();
}

//void Client::getServers() {
//  assert ((_initialised == true) && "Client not initialised");
//  if (!_meta) {
//    // TODO put this into a config file
//    std::string metaserver = "metaserver.worldforge.org";
//    _meta = new Eris::Meta("", metaserver, 10);
//    _meta->GotServerCount.connect(SigC::slot(*this, &Client::gotServerCount));
//    _meta->Failure.connect(SigC::slot(*this, &Client::gotFailure));
//    _meta->ReceivedServerInfo.connect(SigC::slot(*this, &Client::receivedServerInfo));
//    _meta->CompletedServerList.connect(SigC::slot(*this, &Client::completedServerList));
//  }
//  _meta->refresh();
//}

//void Client::stopServers() {
//  assert ((_initialised == true) && "Client not initialised");
//  if (_meta) _meta->cancel();
//}

//void Client::gotServerCount(int count) {
//  Log::writeLog(std::string("Server count: ") + string_fmt(count), Log::LOG_INFO);
//}

//void Client::gotFailure(const std::string &msg) {
//  Log::writeLog(msg, Log::LOG_ERROR);
//}

//void Client::receivedServerInfo(const Eris::ServerInfo & sInfo) {
//  std::cout << "Got serverinfo:\n\r"
//  << "Hostname: " <<sInfo.getHostname()
//  << "\n\rServerName: "<<sInfo.getServername()
//  << "\n\rRuleset: "<<sInfo.getRuleset()
//  << "\n\rServer Type: "<<sInfo.getServer()
//  << "\n\rClients: "<<sInfo.getNumClients()
//  << " Ping: "<< sInfo.getPing()
//  << " Uptime: "<< sInfo.getUptime()
//  << std::endl;
//}

//void Client::completedServerList() {
//  Eris::ServerList l = _meta -> getGameServerList ();
//  Log::writeLog("Listing hostnames...", Log::LOG_INFO);
//  for(Eris::ServerList::iterator i = l.begin(); i != l.end(); ++i) {
//    Eris::ServerInfo inf = *i;
//    Log::writeLog(std::string("Hostname: ") + (i)->getHostname(), Log::LOG_INFO);
//    //HINT: Always use .c_str() for compatibility to MSVC
//  }
//  return;
//}

//void Client::GotTime(double time) {
//  System::instance()->updateTime(time);
//}

void Client::AvatarSuccess(Eris::Avatar *avatar) {
  std::cout << "Avatar sucessfully created" << std::endl;
  m_avatar = avatar;
  _system->setState(SYS_IN_WORLD, true);
  _system->setCharacter(new Character(m_avatar));
Character *c = _system->getCharacter();
  m_avatar->GotCharacterEntity.connect(SigC::slot(*c,
 &Character::GotCharacterEntity));
}

void Client::AvatarFailure(const std::string &msg) {
  std::cerr << msg << std::endl;
}



} /* namespace Sear */
