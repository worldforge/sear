// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "client.h"

#include <Eris/Connection.h>
#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/Person.h>
#include <Eris/PollDefault.h>
#include <sigc++/object.h>
#include <sigc++/slot.h>
#include <sigc++/object_slot.h>
#include <sigc++/bind.h>
#include <iostream.h>

#include "WorldEntity.h"
#include "Console.h"
#include "Factory.h"
#include "Character.h"

#include "System.h"
#include "EventHandler.h"
#include "Event.h"
#include "Render.h"

#ifdef DEBUG
  #define DEBUG_ERIS 1
  #define ERIS_LOG_LEVEL 1
#elif defined(NDEBUG)
  #define DEBUG_ERIS 0
  #define ERIS_LOG_LEVEL 0
#else
  #define DEBUG_ERIS 0
  #define ERIS_LOG_LEVEL 0
#endif

/*
 * The Constructor. Creates the initial Eris Connection object and
 * 
 * @param client_name - Name of client to pass to Eris
*/
Client::Client(System *system, const std::string &client_name) :
  _system(system),
  _connection(NULL),
  _player(NULL),
  _lobby(NULL),
  _status(CLIENT_STATUS_DISCONNECTED),
  _client_name(client_name)
{
}

bool Client::init() {
  _connection = new Eris::Connection(_client_name, DEBUG_ERIS);
  if (!_connection) {
    std::cerr << "Client:: Error - Connection is NULL" << std::endl;	  
    return false;
  }
  _connection->Failure.connect(SigC::slot(this, &Client::NetFailure));
  _connection->Connected.connect(SigC::slot(this, &Client::NetConnected));
  _connection->Disconnected.connect(SigC::slot(this, &Client::NetDisconnected));
  _connection->Disconnecting.connect(SigC::slot(this, &Client::NetDisconnecting));
  _connection->Timeout.connect(SigC::slot(this, &Client::Timeout));
  _connection->StatusChanged.connect(SigC::slot(this, &Client::StatusChanged));

  Eris::setLogLevel((Eris::LogLevel) ERIS_LOG_LEVEL);
  Eris::Logged.connect(SigC::slot(this, &Client::Log));
  return true;
}

void Client::shutdown() {
  std::cout << "Shutting down client." << std::endl;
  if (_player) delete _player;
  if (_connection) delete _connection;
}

void Client::Log(Eris::LogLevel lvl, const std::string &msg) {
  std::string entry = "Eris Log: " + msg;	
  DEBUG_PRINT(entry);
}

int Client::connect(const std::string &host, int port) {
  std::cout << "Client: Connecting to " << host << " on port " << port << std::endl;
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  if (_connection->isConnected()) return CLIENT_ERROR_ALREADY_CONNECTED;
  if (host.empty()) return CLIENT_ERROR_HOSTNAME;
  _system->pushMessage(CLIENT_CONNECTING, CONSOLE_MESSAGE);
  DEBUG_PRINT("Client::connect: Calling connect");
  try {
    _connection->connect(host, port);
  } catch (Eris::InvalidOperation io) {
    DEBUG_PRINT("Client::connect: Caught InvalidOperation: " + io._msg);
    return CLIENT_ERROR_CONNECTING;
  } catch (...) {
    DEBUG_PRINT("Client::connect: Caught Unknown Exception");
    return CLIENT_ERROR_CONNECTING;
  }
  return 0;
}

int Client::reconnect() {
  DEBUG_PRINT("Client::reconnect");
  _system->pushMessage(CLIENT_RECONNECTING, CONSOLE_MESSAGE);
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  //Is this a valid check?
  if (!_connection->isConnected()) return CLIENT_ERROR_NOT_CONNECTED;
  try {
    _connection->reconnect();
  } catch (Eris::InvalidOperation io) {
    DEBUG_PRINT("Client::reconnect: Caught InvalidOperation: " + io._msg);
    return CLIENT_ERROR_CONNECTING;
  } catch (...) {
    DEBUG_PRINT("Client::reconnect: Caught Unknown Exception");
    return CLIENT_ERROR_CONNECTING;
  }
  return 0;
}

int Client::disconnect() {
  DEBUG_PRINT("Client::disconnect");
  _system->pushMessage(CLIENT_DISCONNECTING, CONSOLE_MESSAGE);
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  //Are we logged in?
  if (_player != NULL) {
    DEBUG_PRINT("Client::disconnect: Logging out first...");
    logout();
  }
  try {
    _connection->disconnect();
  } catch (Eris::InvalidOperation io) {
    DEBUG_PRINT("Client::disconnect: Caught InvalidOperation: " + io._msg);
    return CLIENT_ERROR_DISCONNECTING;
  } catch (...) {
    DEBUG_PRINT("Client::disconnect: Caught Unknown Exception");
    return CLIENT_ERROR_DISCONNECTING;
  }
  return 0;
}

int Client::createAccount(const std::string &username, const std::string &fullname, const std::string &password) {
  DEBUG_PRINT("Client::createAccount");
  if (_status != CLIENT_STATUS_CONNECTED) return 9999;
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  if (!_connection->isConnected()) return CLIENT_ERROR_NOT_CONNECTED;
  if (username.empty()) return CLIENT_ERROR_USERNAME;
  if (fullname.empty()) return CLIENT_ERROR_FULLNAME;
  if (password.empty()) return CLIENT_ERROR_PASSWORD;
  if (_status != CLIENT_STATUS_CONNECTED) return 9999;

  //Have we already tried logging in?
  DEBUG_PRINT("Client::createAccount: Creating player");
  if (_player == NULL) {
    _player = new Eris::Player(_connection);
    _player->LoginFailure.connect(SigC::slot(this, &Client::LoginFailure));
    _player->LoginSuccess.connect(SigC::slot(this, &Client::LoginSuccess));
    _player->LogoutComplete.connect(SigC::slot(this, &Client::LogoutComplete));
    _player->GotAllCharacters.connect(SigC::slot(this, &Client::GotAllCharacters));
    _player->GotCharacterInfo.connect(SigC::slot(this, &Client::GotCharacterInfo));

  }
  try {
    _player->createAccount(username, fullname, password);
  } catch (Eris::InvalidOperation e) {
    //Reset variables used here
    DEBUG_PRINT("Client::createAccount: Exception caught");
    std::cerr << "Invalid Operation: " << e._msg << std::endl;
  }
  return 0;
}

int Client::login(const std::string &username, const std::string &password) {
  DEBUG_PRINT("Client::login");
  if (_status != CLIENT_STATUS_CONNECTED) return 9999;
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  if (!_connection->isConnected()) return CLIENT_ERROR_NOT_CONNECTED;
  if (username.empty()) return CLIENT_ERROR_USERNAME;
  if (password.empty()) return CLIENT_ERROR_PASSWORD;
  if (_status != CLIENT_STATUS_CONNECTED) return 9999;
//  std::string msg = std::string(CLIENT_LOGGING_IN) + std::string(": ") + username; 
//  _system->pushMessage(msg);
  DEBUG_PRINT("Client::login: Creating player");
  
  if (_player == NULL) {
    _player = new Eris::Player(_connection);
    _player->LoginFailure.connect(SigC::slot(this, &Client::LoginFailure));
    _player->LoginSuccess.connect(SigC::slot(this, &Client::LoginSuccess));
    _player->LogoutComplete.connect(SigC::slot(this, &Client::LogoutComplete));
    _player->GotAllCharacters.connect(SigC::slot(this, &Client::GotAllCharacters));
    _player->GotCharacterInfo.connect(SigC::slot(this, &Client::GotCharacterInfo));

  }
  try {
    _player->login(username, password);
  } catch (Eris::InvalidOperation e) {
    DEBUG_PRINT("Client::login: Caught Exception - Login Failed");
    std::cerr << "Invalid Operation: " << e._msg << std::endl;
    return CLIENT_ERROR_LOGIN;
  } catch(...) {
    DEBUG_PRINT("Client::login: Caught Exception");
    std::cerr << "Other Error" << std::endl;
    return 9999;
  }
  return 0;
}

void Client::poll() {
//  try {
//    if (_connection != NULL) _connection->poll();
            
    Eris::PollDefault::poll();   
//  } catch (...) {
  //  DEBUG_PRINT("Caught some error - ignoring");
//  }
}

void Client::NetFailure(const std::string &msg)  {
  DEBUG_PRINT("Client::NetFailure");
  _system->pushMessage("Network Failure: " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_DISCONNECTED;
  _system->setState(SYS_CONNECTED, false);
  _system->setState(SYS_LOGGED_IN, false);
  _system->setState(SYS_IN_WORLD, false);
}

void Client::NetConnected() {
  DEBUG_PRINT("Client:NetConnected");
  _system->pushMessage(CLIENT_CONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_CONNECTED;
  try {
    _lobby = Eris::Lobby::instance();
    _lobby->LoggedIn.connect(SigC::slot(this, &Client::LoggedIn));
    _lobby->SightPerson.connect(SigC::slot(this, &Client::SightPerson));
  } catch (Eris::InvalidOperation io) {
    std::cout << "Invalid Operation" << io._msg << std::endl;
  }
  _system->setState(SYS_CONNECTED, true);
}

void Client::NetDisconnected() {
  DEBUG_PRINT("Client::NetDisconnected");
  _system->pushMessage(CLIENT_DISCONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_DISCONNECTED;
  _system->setState(SYS_CONNECTED, false);
}

bool Client::NetDisconnecting() {
  DEBUG_PRINT("Client::Disconnecting");
  _system->pushMessage(CLIENT_DISCONNECTING,1);;
  return false;
}

void Client::LoggedIn(const Atlas::Objects::Entity::Player &p) {
  DEBUG_PRINT("Client::LoggedIn");
  _system->pushMessage(CLIENT_LOGGED_IN, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_LOGGED_IN;
//  _system->setState(SYS_LOGGED_IN, true);
}

void Client::LoginSuccess() {
  DEBUG_PRINT("Client::LoginSuccess");
  _system->pushMessage("Login Success", CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_LOGGED_IN;
//  _system->setState(SYS_LOGGED_IN, true);
}

int Client::createCharacter(const std::string &name, const std::string &type, const std::string &sex, const std::string &description) {
  if (_status != CLIENT_STATUS_LOGGED_IN) return 9999;
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  if (!_connection->isConnected()) return CLIENT_ERROR_NOT_CONNECTED;
  if (name.empty()) return CLIENT_ERROR_CHAR_NAME;
  if (description.empty()) return CLIENT_ERROR_CHAR_DESC;
  if (_player == NULL) return CLIENT_ERROR_PLAYER;
  if (_lobby == NULL) return CLIENT_ERROR_LOBBY;
  std::cout << "Client: Creating character - Name: " << name << " Type: " << type << " Sex: " << sex << " Description: " << description << std::endl;
  _system->pushMessage("Creating Character: " +  name, CONSOLE_MESSAGE);
  Atlas::Objects::Entity::GameEntity ch = Atlas::Objects::Entity::GameEntity::Instantiate();
  Atlas::Message::Object::ListType prs(1, Atlas::Message::Object(type));
  ch.SetParents(prs);
  ch.SetName(name);
  ch.SetAttr("sex", sex);
  ch.SetAttr("description", description);
  Eris::World *world = NULL;
  try {
    world = _player->createCharacter(ch);
  } catch (Eris::InvalidOperation ie) {
    DEBUG_PRINT("Client::createCharacter: Creation failed");
    _system->pushMessage("Character Creation Failed: " +  ie._msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
   return CLIENT_ERROR_CHAR_CREATE;
  } catch (...){
    DEBUG_PRINT("Client::createCharacter: Caught Exception");
    _system->pushMessage("Unknown Create Character Error", CONSOLE_MESSAGE | SCREEN_MESSAGE);
    return 9999;
  }
  DEBUG_PRINT("Client::createCharacter: Setting up callbacks");
  world->Entered.connect(SigC::slot(this, &Client::Entered));
  world->EntityCreate.connect(SigC::slot(this, &Client::EntityCreate));
  world->EntityDelete.connect(SigC::slot(this, &Client::EntityDelete));
  world->Appearance.connect(SigC::slot(this, &Client::Appearance));
  world->Disappearance.connect(SigC::slot(this, &Client::Disappearance));
  world->registerFactory(new Factory());
  return 0;
}

int Client::takeCharacter(const std::string &id) {
  DEBUG_PRINT("Client::takeCharacter");
  if (_status != CLIENT_STATUS_LOGGED_IN) return 9999;
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  if (!_connection->isConnected()) return CLIENT_ERROR_NOT_CONNECTED;
  if (_player == NULL) return CLIENT_ERROR_PLAYER;
  if (_lobby == NULL) return CLIENT_ERROR_LOBBY;
  if (id.empty()) return CLIENT_ERROR_CHAR_ID;

  DEBUG_PRINT("Client::takeCharacter: Taking character - " + id);
  _system->pushMessage(std::string(CLIENT_TAKE_CHARACTER) + std::string(": ") + id, CONSOLE_MESSAGE);
  
  Eris::World *world = NULL;
  try {
    world = _player->takeCharacter(id);
  } catch (Eris::InvalidOperation ie) {
    DEBUG_PRINT("Client::takeCharacter: Error");
    _system->pushMessage("Error Taking Character: " + ie._msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
    return CLIENT_ERROR_CHAR_TAKE;
  } catch (...) {
    DEBUG_PRINT("Client::takeCharacter: Unspecified Error");
    _system->pushMessage("Unknown Take Character Error", CONSOLE_MESSAGE | SCREEN_MESSAGE);
    return 9999;
  }
  DEBUG_PRINT("Client::takeCharacter: Setting up callbacks");
  world->Entered.connect(SigC::slot(this, &Client::Entered));  
  world->EntityCreate.connect(SigC::slot(this, &Client::EntityCreate));
  world->EntityDelete.connect(SigC::slot(this, &Client::EntityDelete));
  world->Appearance.connect(SigC::slot(this, &Client::Appearance));
  world->Disappearance.connect(SigC::slot(this, &Client::Disappearance));
  world->registerFactory(new Factory());
  return 0;
}


void Client::SightPerson(Eris::Person *person) {
  DEBUG_PRINT("Client::SightPerson: " + person->getName() );
  _system->pushMessage(std::string(CLIENT_SIGHTED_PERSON) + " " + person->getName(), CONSOLE_MESSAGE);
//  DEBUG_PRINT(person->getName());
//  DEBUG_PRINT(person->getAccount());
  // This is here dur to bugs in ERIS ??,
  _system->setState(SYS_LOGGED_IN, true);
}

int Client::logout() {
  DEBUG_PRINT("Client::logout");
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  if (!_connection->isConnected()) return CLIENT_ERROR_NOT_CONNECTED;
  if (_player == NULL) return CLIENT_ERROR_PLAYER;
  if (_status == CLIENT_STATUS_LOGGED_IN) {
    _system->pushMessage(CLIENT_LOGGING_OUT,CONSOLE_MESSAGE);
    DEBUG_PRINT("Client::logout: Logging Out");
    _player->logout();
  }
  return 0;
}

int Client::getCharacters() {
  _player->refreshCharacterInfo();
  return 0;
}

std::string Client::getStatus() {
  DEBUG_PRINT("Client::getStatus");
  if (_connection == NULL) return "NO OBJECT";
  switch(_connection->getStatus()) {
    case 0: return "INVALID_STATUS";
    case 1: return "NEGOTIATE";
    case 2: return "CONNECTING";
    case 3: return "CONNECTED";
    case 4: return "DISCONNECTED";
    case 5: return "DISCONNECTING";
    default: return "OTHER";
  }
}

int Client::listRooms() {
  DEBUG_PRINT("Client::listRooms");
  if (_connection == NULL) return CLIENT_ERROR_CONNECTION;
  if (!_connection->isConnected()) return CLIENT_ERROR_NOT_CONNECTED;
  if (_player == NULL) return CLIENT_ERROR_PLAYER;
  if (_lobby == NULL) return CLIENT_ERROR_LOBBY;
  _system->pushMessage("Room Listings:", CONSOLE_MESSAGE);
  DEBUG_PRINT("Client::listRooms: Start of list");
  Eris::StringList i = _lobby->getRooms();
  for (Eris::StringList::iterator I=i.begin(); I != i.end(); I++) {
    _system->pushMessage(I->c_str(), CONSOLE_MESSAGE);
  }
  DEBUG_PRINT("End of List");
  return 0;
}

void Client::EntityCreate(Eris::Entity* e) {
  std::cout << "Entity Created: " << e->getName() << std::endl;
  ((WorldEntity *)e)->handleMove();
}

void Client::EntityDelete(Eris::Entity* e){
  std::cout << "Entity Deleted: " << e->getName() << std::endl;
}

void Client::Entered(Eris::Entity* e){
  DEBUG_PRINT("Entered World");
  _system->setCharacter(new Character((WorldEntity*)Eris::World::Instance()->getFocusedEntity(), _system));
  _status = CLIENT_STATUS_IN_WORLD;
  _system->setState(SYS_IN_WORLD, true);
  _system->getEventHandler()->addEvent(Event(EF_HANDLE_MOVE, e, EC_IN_WORLD, 0));
  
}

void Client::Appearance(Eris::Entity *e){
  std::cout << "Appearance: " << e->getName() << std::endl;
  ((WorldEntity *)e)->handleMove();
  _system->getRenderer()->setModelInUse(e->getID(), true);
}

void Client::Disappearance(Eris::Entity *e){
  std::cout << "Disappearance: " << e->getName() << std::endl;
  _system->getRenderer()->setModelInUse(e->getID(), false);
  // Delete model after 1 minute
  _system->getEventHandler()->addEvent(Event(EF_FREE_MODEL, Event::stringToObject(e->getID()), EC_TIME, 60000 + System::instance()->getTime()));
}

void Client::RootEntityChanged(Eris::Entity *e) {
  DEBUG_PRINT("RootEntityChanged: " +  e->getID());
}

void Client::Timeout(Eris::Connection::Status s) {
  _system->pushMessage("Error: Timeout", 3);
  DEBUG_PRINT(CLIENT_TIMEOUT);
}

void Client::StatusChanged(Eris::Connection::Status s) {
  DEBUG_PRINT(CLIENT_STATUS_CHANGED);
}

void Client::LoginFailure(Eris::LoginFailureType failure_type, const std::string& msg) {
  std::string mesg;
  switch (failure_type) {
    case 0:  mesg = "Invalid LoginFailureType";                   break;
    case 1:  mesg = "Duplicate Account";                          break;
    case 2:  mesg = "Bad Account";                                break;
    case 3:  mesg = "Unknown Account";                            break;
    case 4:  mesg = "Bad Password";                               break;
    case 5:  mesg = "Duplicate Connect - Account already active"; break;
    default: mesg = "Unknown Eris::LoginFailureType";             break;
  }
  std::cout << "Login Failure: " << mesg << " - " << msg <<std::endl;
}

void Client::LogoutComplete(bool clean_logout) {
  DEBUG_PRINT("LOGGED OUT");
//  delete _player;
  if (clean_logout) {
    std::cout << "Client: Clean Logout" << std::endl;
    //Logged out cleanly
  }  else {
    std::cout << "Client: Error in logging out" << std::endl;
    // Emitted due to timeout or other error
  }
  _system->pushMessage(CLIENT_LOGGED_OUT, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  _status = CLIENT_STATUS_CONNECTED;
  _system->setState(SYS_LOGGED_IN, false);
}

void Client::GotCharacterInfo(const Atlas::Objects::Entity::GameEntity& ge) {
  std::cout << "Got Char - Name: " << ge.GetName() << " ID: " << ge.GetId() << std::endl;
}

void Client::GotAllCharacters() {
  DEBUG_PRINT("Client::getCharacters");
  if (_connection == NULL) return;
  if (!_connection->isConnected()) return;
  if (_player == NULL) return;
  if (_lobby == NULL) return;
  Eris::CharacterList l = _player->getCharacters();
  for (Eris::CharacterList::iterator I=l.begin(); I != l.end(); I++) {
    Atlas::Objects::Entity::GameEntity ge = *I;
    _system->pushMessage(ge.GetId().c_str(), CONSOLE_MESSAGE);
  }
}
