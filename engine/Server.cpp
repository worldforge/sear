// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Server.cpp,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#include "Server.h"

#include <iostream>

#include <Eris/Avatar.h>
#include <Eris/PollDefault.h>

#include "script/ScriptEngine.h"
#include "engine/World.h"

#define DEBUG_ERIS     (1)
#define ERIS_LOG_LEVEL (1)

namespace Sear {

static const bool debug = true;

Server *Server::_instance = NULL;

Server::Server() :
  _initialised(false),
  _connection(NULL)
{
  _instance = this;
}

Server::~Server() {
  if (_initialised) shutdown();
  _instance = NULL;
}

void Server::init() {
  if (_initialised) shutdown();
  // Setup logging info
  if (debug) {
    Eris::setLogLevel((Eris::LogLevel)ERIS_LOG_LEVEL);
    Eris::Logged.connect(SigC::slot(*this, &Server::ErisLog));
  }
  
  // Create new connection object
  _connection = new Eris::Connection("Sear", DEBUG_ERIS);
  
  // Connect up connection callbacks
  _connection->Failure.connect(SigC::slot(*this, &Server::NetFailure));
  _connection->Connected.connect(SigC::slot(*this, &Server::NetConnected));
  _connection->Disconnected.connect(SigC::slot(*this, &Server::NetDisconnected));
  _connection->Disconnecting.connect(SigC::slot(*this, &Server::NetDisconnecting));
  _connection->Timeout.connect(SigC::slot(*this, &Server::NetTimeout));
  _connection->StatusChanged.connect(SigC::slot(*this, &Server::NetStatusChanged));
  // Create player object
  _player = new Eris::Player(_connection);
  // Set up player signals
  _player->LoginFailure.connect(SigC::slot(*this, &Server::NetLoginFailure));
  _player->LoginSuccess.connect(SigC::slot(*this, &Server::NetLoginSuccess));
  _player->LogoutComplete.connect(SigC::slot(*this, &Server::NetLogoutComplete));
  _player->GotAllCharacters.connect(SigC::slot(*this, &Server::NetGotAllCharacters));
  _player->GotCharacterInfo.connect(SigC::slot(*this, &Server::NetGotCharacterInfo));

  _initialised = true;
}

void Server::shutdown() {
  if (_world) {
    _world->shutdown();
    delete _world;
    _world = NULL;
  }
  if (_connection) {
    // TODO Remove Signals
//    delete _connection;
    _connection = NULL;
  }
  _initialised = false;
}


void Server::connect(const std::string &host, short port) {
  if (debug) std::cout << "Connecting to " << host << " on port " << port << std::endl;
  if (_connection->isConnected()) {
    if (debug) std::cout << "Error - Already connected to server" << std::endl;
    throw std::exception();
  }
  try {
    _connection->connect(host, port);
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::reconnect() {
  if (debug) std::cout << "Reconnecting to server" << std::endl;
  if (!_connection->isConnected()) {
    std::cerr << "Error, we need to be connected before a reconnect" << std::endl;
    throw std::exception();
  }
  try {
    _connection->reconnect();
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::disconnect() {
  if (debug) std::cout << "Disconnecting from server" << std::endl;
  if (!_connection->isConnected()) {
    std::cerr << "Error, we need to be connected before a disconnect" << std::endl;
    throw std::exception();
  }
  try {
    _connection->disconnect();
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::createAccount(const std::string &user_name, const std::string &full_name, const std::string &password) {
  if (!_connection->isConnected()) {
    std::cerr << "Error, we need to be connected first" << std::endl;
    throw std::exception();
  }
  try {
    _player->createAccount(user_name, full_name, password);
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}


void Server::login(const std::string &user_name, const std::string &password) {
  if (!_connection->isConnected()) {
    std::cerr << "Error, we need to be connected first" << std::endl;
    throw std::exception();
  }
  try {
    _player->login(user_name, password);
 } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::logout() {
  if (!_connection->isConnected()) {
    std::cerr << "Error, we need to be connected first" << std::endl;
    throw std::exception();
  }
  try {
    _player->logout();
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}


void Server::getCharacters() {
  if (!_connection->isConnected()) {
    std::cerr << "Error, we need to be connected first" << std::endl;
    throw std::exception();
  }
  try {
    _player->refreshCharacterInfo();
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::createCharacter(CharacterStruct &cs) {
  Atlas::Objects::Entity::GameEntity ch = Atlas::Objects::Entity::GameEntity::Instantiate();
  Atlas::Message::Object::ListType prs(1, Atlas::Message::Object(cs.type));
  ch.SetParents(prs);
  ch.SetName(cs.name);
  ch.SetAttr("sex", cs.sex);
  ch.SetAttr("description", cs.description);

  Eris::World *world = NULL;
  try {
    world = _player->createCharacter(ch)->getWorld();
    _world = new World();
    _world->init(world);
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::takeCharacter(const std::string &id) {
  Eris::World *world = NULL;
  try {
    world = _player->takeCharacter(id)->getWorld();
    _world = new World();
    _world->init(world);
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::ErisLog(Eris::LogLevel lvl, const std::string &msg) {
  if (debug) std::cout << "Eris: " << msg << std::endl;
}

// Connection Callbacks
void Server::NetFailure(const std::string &msg) {
  std::cerr << "Network Failure: " << msg << std::endl;
  ScriptEngine::instance()->event("onNetFailure");
}

void Server::NetConnected() {
  if (debug) std::cout << "Connected" << std::endl;
  ScriptEngine::instance()->event("onNetConnect");
}

void Server::NetDisconnected() {
  if (debug) std::cout << "Disconnected" << std::endl;
  ScriptEngine::instance()->event("onDisconnect");
}

bool Server::NetDisconnecting() {
  if (debug) std::cout << "Disconnecting" << std::endl;
  ScriptEngine::instance()->event("onNetDisconnecting");
  return false;
}

void Server::NetTimeout(Eris::Connection::Status) {
  if (debug) std::cout << "Network Timeout" << std::endl;
  ScriptEngine::instance()->event("onNetTimeout");
}

void Server::NetStatusChanged(Eris::Connection::Status s) {
  std::string msg;
  switch (s) {
    case (Eris::Connection::INVALID_STATUS): msg = "Invalid Status"; break;
    case (Eris::Connection::NEGOTIATE): msg = "Negotiate"; break;
    case (Eris::Connection::CONNECTING): msg = "Connecting"; break;
    case (Eris::Connection::CONNECTED): msg = "Connected"; break;
    case (Eris::Connection::DISCONNECTED): msg = "Disconncted"; break;
    case (Eris::Connection::DISCONNECTING): msg = "Disconnecting"; break;
    case (Eris::Connection::QUERY_GET): msg = "Query Get"; break;
    default: msg = "Unknown status"; break;
  }
  std::cout << "Status Changed to: " << msg << std::endl;
  ScriptEngine::instance()->event("onNetStatusChanged");
}
  // Player Callbacks
void Server::NetLoginSuccess() {
  if (debug) std::cout << "Login Sucess" << std::endl;
  ScriptEngine::instance()->event("onNetLoginSuccess");
}

void Server::NetLoginFailure(Eris::LoginFailureType failure, const std::string &msg) {
  std::string mesg;
  switch (failure) {
    case (Eris::LOGIN_INVALID): mesg = "Login Invalid"; break;
    case (Eris::LOGIN_DUPLICATE_ACCOUNT): mesg = "Duplicate Account"; break;
    case (Eris::LOGIN_BAD_ACCOUNT): mesg = "Bad Account"; break;
    case (Eris::LOGIN_UNKNOWN_ACCOUNT): mesg = "Unknown Account"; break;
    case (Eris::LOGIN_BAD_PASSWORD): mesg = "Bad Password"; break;
    case (Eris::LOGIN_DUPLICATE_CONNECT): mesg = "Duplicate Connect"; break;
    default: mesg = "Uknonwn failure type"; break;
  }
  std::cout << "LoginFailure: " << mesg << " - " << msg << std::endl;
  ScriptEngine::instance()->event("onNetLoginFailure");
}

void Server::NetLogoutComplete(bool clean_logout) {
  if (clean_logout) {
    if (debug) std::cout << "Logged out cleanly" << std::endl;
  } else {
    if (debug) std::cout << "Unclean logout" << std::endl;
  }
  ScriptEngine::instance()->event("onNetLogoutComplete");
}

void Server::NetGotCharacterInfo(const Atlas::Objects::Entity::GameEntity &ge) {
  if (debug) {
    std::cout << "Got Character " << ge.GetName() << std::endl;
    std::cout << "ID: " << ge.GetId() << std::endl;
    std::cout << "Type: " << ge.GetObjtype() << std::endl;
  }
  ScriptEngine::instance()->event("onNetGotCharacterInfo");
}
void Server::NetGotAllCharacters() {
  if (debug) std::cout << "Got all characters" << std::endl;
  ScriptEngine::instance()->event("onNetGotAllCharacters");
}

void Server::poll() {
  try {
    Eris::PollDefault::poll();
  } catch (Eris::InvalidOperation &io) {
    std::cerr << io._msg << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Caught std::Exception" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;	  
  }
}

void Server::registerCommands(lua_State *L) {
  lua_register(L, "connect", &Server::l_connect);
  lua_register(L, "disconnect", &Server::l_disconnect);
  lua_register(L, "reconnect", &Server::l_reconnect);
  lua_register(L, "create_account", &Server::l_create_account);
  lua_register(L, "login", &Server::l_login);
  lua_register(L, "logout", &Server::l_logout);
  lua_register(L, "create_character", &Server::l_create_character);
  lua_register(L, "take_character", &Server::l_take_character);
  lua_register(L, "get_characters", &Server::l_get_characters);
}

int Server::l_connect(lua_State *L) {
  int top = lua_gettop(L);
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  if (top == 2) {
    std::string host = lua_tostring(L, 1);
    short port = (short)lua_tonumber(L, 2);
    server->connect(host, port);
  } else if (top == 1) {
    std::string host = lua_tostring(L, 1);
    server->connect(host);
  } else {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_disconnect(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  try {
    server->disconnect();
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_reconnect(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  try {
    server->reconnect();
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_create_account(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  int top = lua_gettop(L);
  if (top != 3) {
    lua_pushnumber(L, 1);
    return 1;
  }
  std::string username = lua_tostring(L, 1);
  std::string fullname = lua_tostring(L, 2);
  std::string password = lua_tostring(L, 3);
  try {
    server->createAccount(username, fullname, password);
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_login(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  int top = lua_gettop(L);
  if (top != 2) {
    lua_pushnumber(L, 1);
    return 1;
  }
  std::string username = lua_tostring(L, 1);
  std::string password = lua_tostring(L, 2);
  try {
    server->login(username, password);
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_logout(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  try {
    server->logout();
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_create_character(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  int top = lua_gettop(L);
  if (top != 4) {
    lua_pushnumber(L, 1);
    return 1;
  }
  
  CharacterStruct ch;
  ch.name = lua_tostring(L, 1);
  ch.type = lua_tostring(L, 2);
  ch.sex = lua_tostring(L, 3);
  ch.description = lua_tostring(L, 4);
  try {
    server->createCharacter(ch);
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_take_character(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  int top = lua_gettop(L);
  if (top != 1) {
    lua_pushnumber(L, 1);
    return 1;
  }
  std::string id = lua_tostring(L, 1);
  try {
    server->takeCharacter(id);
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

int Server::l_get_characters(lua_State *L) {
  Server *server = Server::instance();
  if (!server) {
    lua_pushnumber(L, 1);
    return 1;
  }
  try {
    server->getCharacters();
  } catch (...) {
    lua_pushnumber(L, 1);
    return 1;
  }
  lua_pushnumber(L, 0);
  return 1;
}

} /* namespace Sear */
