// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall, University of Southampton

#include "client.h"

#include <sigc++/object_slot.h>

#include <Atlas/Objects/Entity.h>

#include <Eris/Avatar.h>
#include <Eris/Account.h>
#include <Eris/DeleteLater.h>
#include <Eris/Log.h>
#include <Eris/Entity.h>
#include <Eris/PollDefault.h>
#include <Eris/Exceptions.h>
#include <Eris/View.h>
#include <Eris/Types.h>

#include "common/Utility.h"

#include "ActionHandler.h"
#include "Calendar.h"
#include "Console.h"
#include "Character.h"
#include "CharacterManager.h"
#include "Factory.h"
#include "System.h"
#include "WorldEntity.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {


//  Small class to pass a smart pointer to Eris::deleteLater
template<class T>
class DelLater {
public:
  DelLater(const SPtr<T> &s) :
    ptr(s){
  }
  ~DelLater() {
  }
private:
  SPtr<T> ptr;
};

static const int DEFAULT_PORT = 6767;

// Console Command strings
static const std::string SERVER_CONNECT = "connect";
static const std::string SERVER_DISCONNECT = "disconnect";
static const std::string ACCOUNT_CREATE = "create";
static const std::string ACCOUNT_LOGIN = "login";
static const std::string ACCOUNT_LOGOUT = "logout";
static const std::string LEAVE_WORLD = "leave";

// Config items
static const std::string SECTION_CONNECTION = "connection";
static const std::string KEY_LOGLEVEL = "loglevel";

/*
 * The Constructor. Creates the initial Eris Connection object and
 * 
 * @param client_name - Name of client to pass to Eris
*/
Client::Client(System *system, const std::string &client_name) :
  m_system(system), 
  m_status(CLIENT_STATUS_DISCONNECTED),
  m_client_name(client_name),
  m_initialised(false),
  m_loglevel(Eris::LOG_ERROR)
{
  assert((system != NULL) && "System is NULL");
}

Client::~Client() {
  if (m_initialised) shutdown();
}

bool Client::init() {
  assert(m_initialised == false);

  // Setup logging
  Eris::setLogLevel(m_loglevel);
  Eris::Logged.connect(sigc::mem_fun(this, &Client::Log));

  m_initialised = true;

  return true;
}

void Client::shutdown() {
  assert(m_initialised == true);

  notify_callbacks();

  m_account.release();
  m_connection.release();

  setStatus(CLIENT_STATUS_DISCONNECTED);

  m_initialised = false;
}

void Client::Log(Eris::LogLevel lvl, const std::string &msg) {
  assert(m_initialised == true);
  printf("[ErisLog] %s\n", msg.c_str());
}

int Client::connect(const std::string &host, int port) {
  assert(m_initialised == true);
  if (debug) printf("[Client] Connect\n");

  if (m_status >= CLIENT_STATUS_CONNECTING) {
    m_system->pushMessage("Error: Connection already in progress", CONSOLE_MESSAGE);
    return 1;
  }
  // Check args
  if (host.empty()) {
    m_system->pushMessage("Error: No hostname specified", CONSOLE_MESSAGE);
    return 1;
  }

  if (debug) printf("[Client] Connecting to %s on port %d\n",  host.c_str(), port);

  assert(m_connection.get() == NULL);

  // Create new eris connection object
  m_connection = SPtr<Eris::Connection>(new Eris::Connection(m_client_name, host, port, false));

  // Set up connection callbacks
  m_connection->Failure.connect(sigc::mem_fun(this, &Client::NetFailure));
  m_connection->Connected.connect(sigc::mem_fun(this, &Client::NetConnected));
  m_connection->Disconnected.connect(sigc::mem_fun(this, &Client::NetDisconnected));
  m_connection->Disconnecting.connect(sigc::mem_fun(this, &Client::NetDisconnecting));
  m_connection->StatusChanged.connect(sigc::mem_fun(this, &Client::StatusChanged));
  
  m_system->pushMessage(CLIENT_CONNECTING, CONSOLE_MESSAGE);

  setStatus(CLIENT_STATUS_CONNECTING);
  if (m_connection->connect()  != 0) {
    fprintf(stderr, "[Client] Error: Connection Error\n");
    m_system->pushMessage("Error: Connection Error", CONSOLE_MESSAGE);
    setStatus( CLIENT_STATUS_DISCONNECTED);
    Eris::deleteLater(new DelLater<Eris::Connection>(m_connection));
    m_connection.release();
    return 1;
  }

  return 0;
}

int Client::disconnect() {
  assert ((m_initialised == true) && "Client not initialised");

  if (debug) printf("[Client] disconnect\n");

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
    fprintf(stderr, "[Client] Error: Disconnect Error\n");
    m_system->pushMessage("Error: Disconnect Error", CONSOLE_MESSAGE);
    Eris::deleteLater(new DelLater<Eris::Connection>(m_connection));
    m_connection.release();
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
  if (m_status >= CLIENT_STATUS_LOGGED_IN) {
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

  if (debug) printf("[Client] Creating player\n");
  // m_account will still be around if previous attempts have failed
  if (m_account.get() == NULL) {
    m_account = SPtr<Eris::Account>(new Eris::Account(m_connection.get()));

    System::instance()->getCharacterManager()->setAccount(m_account.get());


    // Setup callbacks
    m_account->LoginFailure.connect(sigc::mem_fun(this, &Client::LoginFailure));
    m_account->LoginSuccess.connect(sigc::mem_fun(this, &Client::LoginSuccess));
    m_account->LogoutComplete.connect(sigc::mem_fun(this, &Client::LogoutComplete));
    m_account->AvatarSuccess.connect(sigc::mem_fun(this, &Client::AvatarSuccess));
    m_account->AvatarFailure.connect(sigc::mem_fun(this, &Client::AvatarFailure));
    m_account->AvatarDeactivated.connect(sigc::mem_fun(this, &Client::AvatarDeactivated));
  }

  setStatus(CLIENT_STATUS_LOGGING_IN);
  Eris::Result res = m_account->createAccount(username, fullname, password);

  switch (res) {
    case Eris::NO_ERR: break;
    case Eris::ALREADY_LOGGED_IN:
    case Eris::NOT_CONNECTED:
    case Eris::NOT_LOGGED_IN:
    case Eris::DUPLICATE_CHAR_ACTIVE:
    case Eris::BAD_CHARACTER_ID:
      assert(false);
      fprintf(stderr, "[Client] We've hit an unexpected return code\n");
      m_system->pushMessage("An Unknown error occured", 
                            CONSOLE_MESSAGE | SCREEN_MESSAGE);
      setStatus(CLIENT_STATUS_CONNECTED);
      // Clean up object just to be safe
      m_account.release();
      return 1;
      break;
  }
  return 0;
}

int Client::login(const std::string &username, const std::string &password) {
  assert ((m_initialised == true) && "Client not initialised");

  if (debug) printf("[Client] login\n");

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
  assert(m_connection.get() != NULL);
  assert(m_connection->isConnected() == true);

  if (username.empty()) {
    m_system->pushMessage("Error: No username specified", CONSOLE_MESSAGE);
    return 1;
  }
  if (password.empty()) {
    m_system->pushMessage("Error: No password specified", CONSOLE_MESSAGE);
    return 1;
  }

  if (debug) printf("[Client] Creating player\n");
  
  if (m_account.get() == NULL) {
    m_account = SPtr<Eris::Account>(new Eris::Account(m_connection.get()));
    System::instance()->getCharacterManager()->setAccount(m_account.get());
    // setup player callbacks
    m_account->LoginFailure.connect(sigc::mem_fun(this, &Client::LoginFailure));
    m_account->LoginSuccess.connect(sigc::mem_fun(this, &Client::LoginSuccess));
    m_account->LogoutComplete.connect(sigc::mem_fun(this, &Client::LogoutComplete));
    m_account->AvatarSuccess.connect(sigc::mem_fun(this, &Client::AvatarSuccess));
    m_account->AvatarFailure.connect(sigc::mem_fun(this, &Client::AvatarFailure));
    m_account->AvatarDeactivated.connect(sigc::mem_fun(this, &Client::AvatarDeactivated));
  }

  setStatus(CLIENT_STATUS_LOGGING_IN);
  Eris::Result res = m_account->login(username, password);

  switch (res) {
    case Eris::NO_ERR: break;
    case Eris::ALREADY_LOGGED_IN:
    case Eris::NOT_CONNECTED:
    case Eris::NOT_LOGGED_IN:
    case Eris::DUPLICATE_CHAR_ACTIVE:
    case Eris::BAD_CHARACTER_ID:
      assert(false);
      fprintf(stderr, "[Client] We've hit an unexpected return code\n");
      m_system->pushMessage("An Unknown error occured", 
                            CONSOLE_MESSAGE | SCREEN_MESSAGE);
      setStatus(CLIENT_STATUS_CONNECTED);
      // Clean up object just to be safe
      m_account.release();
      return 1;
      break;
  }
  return 0;
}

void Client::poll() {
  assert ((m_initialised == true) && "Client not initialised");
  try {
    Eris::PollDefault::poll();
  } catch (Eris::InvalidOperation io) {
    fprintf(stderr,"[Client] Eris Exception: %s\n", io._msg.c_str());
  } catch (Eris::BaseException be) {
    fprintf(stderr, "[Client] Eris Exception: %s\n", be._msg.c_str());
  } catch (std::runtime_error re) {
    fprintf(stderr, "[Client] STD::RUNTIME ERROR\n");
  } catch (...) {
    fprintf(stderr,"[Client] Caught some error - ignoring\n");
  }
}

void Client::NetFailure(const std::string &msg)  {
  printf("[Client] NetFailure\n");
  m_system->pushMessage("Network Failure: " + msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_DISCONNECTED);

  Eris::deleteLater(new DelLater<Eris::Account >(m_account));
  m_account.release();

  Eris::deleteLater(new DelLater<Eris::Connection >(m_connection));
  m_connection.release();

}

void Client::NetConnected() {
  if (debug) printf("[Client] NetConnected\n");
  m_system->pushMessage(CLIENT_CONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_CONNECTED);
}

void Client::NetDisconnected() {
  printf("[Client] NetDisconnected\n");
  m_system->pushMessage(CLIENT_DISCONNECTED, CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_DISCONNECTED);

  Eris::deleteLater(new DelLater<Eris::Account >(m_account));
  m_account.release();

  Eris::deleteLater(new DelLater<Eris::Connection>(m_connection));
  m_connection.release();
}

bool Client::NetDisconnecting() {
  if (debug) printf("[Client] Disconnecting\n");
  m_system->pushMessage(CLIENT_DISCONNECTING,1);;
  return false;
}

void Client::LoginSuccess() {
  if (debug) printf("[Client] LoginSuccess\n");
  m_system->pushMessage("Login Success", CONSOLE_MESSAGE | SCREEN_MESSAGE);
  setStatus(CLIENT_STATUS_LOGGED_IN);
  m_account->refreshCharacterInfo();
}

int Client::leaveWorld()
{
    if (m_status != CLIENT_STATUS_IN_WORLD) {
        m_system->pushMessage("Not in world", 
            CONSOLE_MESSAGE | SCREEN_MESSAGE);
        return 1;
    }
    
    setStatus(CLIENT_STATUS_GOING_OUT_WORLD);

    // Deactive each character
    const Eris::ActiveCharacterMap &acm = m_account->getActiveCharacters();
    while (acm.empty() == false) {
      Eris::ActiveCharacterMap::const_iterator I = acm.begin();
      Eris::Avatar *avatar = I->second;
      m_account->deactivateCharacter(avatar);
    }

    return 0;
}

int Client::logout() {
  assert ((m_initialised == true) && "Client not initialised");
  if (debug) printf("[Client] logout\n");

  if (m_account.get() == NULL) {
    m_system->pushMessage("Error: Not logged in", CONSOLE_MESSAGE);
    return 1;
  }

  if (m_status >= CLIENT_STATUS_LOGGED_IN) {
    m_system->pushMessage(CLIENT_LOGGING_OUT,CONSOLE_MESSAGE);
    printf("[Client] Logging Out\n");
    setStatus(CLIENT_STATUS_LOGGING_OUT);
    m_account->logout();
  }
  return 0;
}

static std::string getStatus(const Eris::Connection::Status &s) {
  switch (s) {
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
  if (debug) printf("[Client] Status Changed: %s\n", getStatus(s).c_str());
}

void Client::LoginFailure(const std::string& msg) {
  fprintf(stderr, "[Client] Login Failure: %s\n", msg.c_str());
  m_system->pushMessage("Login Failure" + msg, CONSOLE_MESSAGE);
  setStatus(CLIENT_STATUS_CONNECTED);
}

void Client::LogoutComplete(bool clean_logout) {
  if (clean_logout) {
    //Logged out cleanly
    printf("[Client] Clean Logout\n");
  }  else {
    // Emitted due to timeout or other error
    fprintf(stderr, "[Client] Error during Logout\n");
  }
  m_system->pushMessage(CLIENT_LOGGED_OUT, CONSOLE_MESSAGE | SCREEN_MESSAGE);

  // We may have already been disconnected, so its not safe to set status
  // back to CONNECTED unless we know we still are.
  if (m_status > CLIENT_STATUS_CONNECTED) {
    setStatus(CLIENT_STATUS_CONNECTED);
  }

  Eris::deleteLater(new DelLater<Eris::Account>(m_account));
  m_account.release();
}

void Client::registerCommands(Console *console) {
  assert ((m_initialised == true) && "Client not initialised");
  console->registerCommand(SERVER_CONNECT, this);
  console->registerCommand(SERVER_DISCONNECT, this);
  console->registerCommand(ACCOUNT_CREATE, this);
  console->registerCommand(ACCOUNT_LOGIN, this);
  console->registerCommand(ACCOUNT_LOGOUT, this);
  console->registerCommand(LEAVE_WORLD, this);
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
  else if (command == LEAVE_WORLD) {
    leaveWorld();
  }
}

void Client::AvatarSuccess(Eris::Avatar *avatar) {
  assert(avatar != NULL);
  printf("[Client] Avatar sucessfully created\n");

  // TODO: Make a member var and clean up correctly
  Factory* f = new Factory(*m_connection->getTypeService());
  avatar->getView()->registerFactory(f);

  avatar->getView()->Appearance.connect(sigc::mem_fun(this, &Client::onEntityAppearance));
  avatar->getView()->Disappearance.connect(sigc::mem_fun(this, &Client::onEntityDisappearance));

  avatar->GotCharacterEntity.connect(sigc::mem_fun(this, &Client::GotCharacterEntity));
}

void Client::AvatarFailure(const std::string &msg) {
  fprintf(stderr, "[Client] AvatarFailure: %s\n", msg.c_str());
  m_system->pushMessage(msg, CONSOLE_MESSAGE | SCREEN_MESSAGE);

  // TODO: Check Current status before falling back
  setStatus(CLIENT_STATUS_LOGGED_IN);

  m_system->getActionHandler()->handleAction("avatar_failed", 0);
}

void Client::AvatarDeactivated(Eris::Avatar* av)
{
  if (debug) printf("[Client] Avatar deactivated: %s\n", av->getId().c_str());
 
  // Change status if we no longer have any active characters. 
  Eris::ActiveCharacterMap acl = m_account->getActiveCharacters();
  if (acl.empty()) {
    setStatus(CLIENT_STATUS_LOGGED_IN);
  }
}

void Client::GotCharacterEntity(Eris::Entity *e) {
  assert(e != NULL);

  if (debug) printf("[Client] Character Entity Found: %s\n", e->getId().c_str());

  // Change status if we have first entity.
  // TODO: What happens if we have activated several chars but this is the first
  //       trigger of the signal? 
  Eris::ActiveCharacterMap acl = m_account->getActiveCharacters();
  if (acl.size() == 1) {
    setStatus(CLIENT_STATUS_IN_WORLD);
  }
}

void Client::setStatus(int status) {
  assert(m_system);

  switch (status) {
    case CLIENT_STATUS_DISCONNECTED:
      m_system->getActionHandler()->handleAction("disconnected", 0);
    case CLIENT_STATUS_DISCONNECTING:
    case CLIENT_STATUS_CONNECTING:
      m_system->setState(SYS_CONNECTED, false);
      m_system->setState(SYS_LOGGED_IN, false);
      m_system->setState(SYS_IN_WORLD, false);
      break;
    case CLIENT_STATUS_CONNECTED:
    case CLIENT_STATUS_LOGGING_IN:
    case CLIENT_STATUS_LOGGING_OUT:
      m_system->setState(SYS_CONNECTED, true);
      m_system->setState(SYS_LOGGED_IN, false);
      m_system->setState(SYS_IN_WORLD, false);
      m_system->getActionHandler()->handleAction("connected", 0);
      break;
    case CLIENT_STATUS_LOGGED_IN:
    case CLIENT_STATUS_GOING_IN_WORLD:
      m_system->setState(SYS_CONNECTED, true);
      m_system->setState(SYS_LOGGED_IN, true);
      m_system->setState(SYS_IN_WORLD, false);
      m_system->getActionHandler()->handleAction("logged_in", 0);
      break;
    case CLIENT_STATUS_GOING_OUT_WORLD:
      m_system->setState(SYS_CONNECTED, true);
      m_system->setState(SYS_LOGGED_IN, true);
      m_system->setState(SYS_IN_WORLD, true);
      break;
    case CLIENT_STATUS_IN_WORLD:
      m_system->setState(SYS_CONNECTED, true);
      m_system->setState(SYS_LOGGED_IN, true);
      m_system->setState(SYS_IN_WORLD, true);
      m_system->getActionHandler()->handleAction("world_entered", 0);
      break;
  }
  m_status = status;
}

void Client::readConfig(varconf::Config &config) {
  if (config.findItem(SECTION_CONNECTION, KEY_LOGLEVEL)) {
    std::string lvl = (std::string)config.getItem(SECTION_CONNECTION, KEY_LOGLEVEL);
    setErisLogLevel(lvl);
  }
}

void Client::writeConfig(varconf::Config &config) const {

}

void Client::setErisLogLevel(const std::string &level) {
  if (level == "error")         m_loglevel = Eris::LOG_ERROR;
  else if (level == "warning")  m_loglevel = Eris::LOG_WARNING;
  else if (level == "notice")   m_loglevel = Eris::LOG_NOTICE;
  else if (level == "verbose")  m_loglevel = Eris::LOG_VERBOSE;
  else if (level == "debug")    m_loglevel = Eris::LOG_DEBUG;
  else {
    fprintf(stderr, "[Client] Unknown eris log level %s\n", level.c_str());
  }
  std::cout << "[Client] Setting log level " << level << std::endl;
  Eris::setLogLevel(m_loglevel);
}

void Client::onEntityAppearance(Eris::Entity *e) {
  WorldEntity *we = dynamic_cast<WorldEntity *>(e);
  we->startFadeIn();
}

void Client::onEntityDisappearance(Eris::Entity *e) {
  WorldEntity *we = dynamic_cast<WorldEntity *>(e);
  we->startFadeOut();
}

} /* namespace Sear */
