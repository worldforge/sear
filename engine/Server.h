// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Server.h,v 1.1.2.1 2003-01-05 14:22:01 simon Exp $

#ifndef SEAR_SERVER_H
#define SEAR_SERVER_H 1

#include <sigc++/object_slot.h>

#include <string>

#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/Player.h>
#include <Atlas/Message/DecoderBase.h>

extern "C" {
  #include <lua.h>
}

namespace Sear {

// Forward Declarations
class World;
	
class Server : public SigC::Object {
public:
  /** Basic Character info for creation
   * The Characer Struct is used to pass construction details
   * into the createCharacter method. This allows additional
   * character attributes to easily be added as required without
   * changing the whole interface.
   */ 	
  typedef struct CharacterStruct {
    std::string name;
    std::string type;
    std::string sex;
    std::string description;
  } CharacterStruct;

  /** Default constructor
   * Initialises varaibles
   */ 	
  Server();

  /** Default destructor
   * Calls shutdown if initialised
   */ 
  ~Server();
  
  /** Initialises Server object
   * This creates the connection and player object and
   * connects up the signals to them
   */ 
  void init();

  /** This closes the Server object down
   * Deletes connection and player object
   */ 
  void shutdown();

  /** Connects to a server
   * Connets to a server host, on the specifed port. If port
   * is not specfied, a default is supplied;
   * \param host The hostname or IP or the server
   * \param port Optional port number to connect on
   */ 
  void connect(const std::string &host, short port = DEFAULT_PORT);

  /** Reconnects to the current server
   * Reconnects to the current server
   */ 
  void reconnect();

  /** Disconnect from current server
   * Disconnet from current server
   */ 
  void disconnect();

  /** Create a new user account
   * Create a new user account on the server.
   * \param user_name Desired user name
   * \param full_name Full name of user
   * \param password Desired password
   */ 
  void createAccount(const std::string &user_name, const std::string &full_name, const std::string &password);

  /** Login to an existing user account
   * Login to an existing user account on the server
   * \param user_name The username of the account
   * \param password The accounts password
   */ 
  void login(const std::string &user_name, const std::string &password);

  /** Logout of account
   * Logs out of account
   */ 
  void logout();

  /** Obtains list of characters owned by account
   * Gets a list of characters that the account owns 
   * from the server
   */ 
  void getCharacters();

  /** Create a new character
   * Creates a new character on the server
   * \param cs The data structure containing the data to 
   * create the new character with
   */ 
  void createCharacter(CharacterStruct &cs);

  /** Use an existing character
   * Use an existing character
   * \param id The ID of the character to use
   */ 
  void takeCharacter(const std::string &id);

  /** Poll Network
   * Polls network connection
   */ 
  void poll();
 
  /**
   * Returns world object
   * \return The world
   */  
  World *getWorld() const { return _world; }
  
  static Server *instance() { return _instance; }
  
  static void registerCommands(lua_State *L);
private:
  // Callbacks
  // Log Callback
  void ErisLog(Eris::LogLevel, const std::string &msg);
  // Connection Callbacks
  void NetFailure(const std::string &msg);
  void NetConnected();
  void NetDisconnected();
  bool NetDisconnecting();
  void NetTimeout(Eris::Connection::Status);
  void NetStatusChanged(Eris::Connection::Status);
  // Player Callbacks
  void NetLoginSuccess();
  void NetLoginFailure(Eris::LoginFailureType, const std::string &msg);
  void NetLogoutComplete(bool );
  void NetGotCharacterInfo(const Atlas::Objects::Entity::GameEntity &ge);
  void NetGotAllCharacters();
  
  bool _initialised;
  Eris::Connection *_connection;
  Eris::Player *_player;
  World *_world;
  static Server *_instance;
  static const short DEFAULT_PORT = 6767;

  
  static int l_connect(lua_State *L);
  static int l_reconnect(lua_State *L);
  static int l_disconnect(lua_State *L);
  static int l_create_account(lua_State *L);
  static int l_login(lua_State *L);
  static int l_logout(lua_State *L);
  static int l_create_character(lua_State *L);
  static int l_take_character(lua_State *L);
  static int l_get_characters(lua_State *L);
};

} /* namespace Sear */

#endif /* SEAR_SERVER_H */
