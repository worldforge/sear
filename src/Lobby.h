// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef SEAR_LOBBY_H
#define SEAR_LOBBY_H 1

#include <sigc++/object_slot.h>
#include "ConsoleObject.h"
#include <Eris/Lobby.h>
#include <Eris/Room.h>

namespace Sear {

// Forward decls
class Console;
	
class Lobby : public ConsoleObject, public SigC::Object {
public:
	
  void init(Eris::Lobby *lobby);
  void say(const std::string &speech);
  void sayPrivate(const std::string &speech, const std::string &to);
  void shutdown();
  void render();
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

protected:	
  void privateChat(const std::string &src, const std::string &msg);
  void Entered(Eris::Room *room);
  void Talk(Eris::Room *room, const std::string &str1, const std::string &str2);
  void Emote(Eris::Room *room, const std::string&, const std::string&);
  void Appearance(Eris::Room*, const std::string&str);
  void Disappearance(Eris::Room*, const std::string&str);
  void Changed(const Eris::StringSet&);

  Eris::Lobby *_lobby;

private:
  static const char * const JOIN_ROOM = "join_room";
  static const char * const LEAVE_ROOM = "leave_room";
  static const char * const LIST_ROOMS = "list_rooms";
  static const char * const TALK = "talk";
  static const char * const EMOTE = "emote";
};

} /* namespace Sear */
#endif /* CONSOLEOBJECT_H */

