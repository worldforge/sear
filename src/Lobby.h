// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Lobby.h,v 1.5 2003-03-23 19:51:49 simon Exp $

#ifndef SEAR_LOBBY_H
#define SEAR_LOBBY_H 1

#include <sigc++/object_slot.h>
#include "interfaces/ConsoleObject.h"
#include <Eris/Lobby.h>
#include <Eris/Room.h>

namespace Sear {

// Forward decls
class Console;
	
class Lobby : public ConsoleObject, public SigC::Object {
public:
  Lobby();
  ~Lobby();
  
  void init(Eris::Lobby *lobby);
  void say(const std::string &speech);
  void emote(const std::string &speech);
  void sayPrivate(const std::string &speech, const std::string &to);
  void shutdown();
  void render();
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

private:
  // Eris callbacks
  void privateChat(const std::string &src, const std::string &msg);
  void Entered(Eris::Room *room);
  void Talk(Eris::Room *room, const std::string &str1, const std::string &str2);
  void Emote(Eris::Room *room, const std::string&, const std::string&);
  void Appearance(Eris::Room*, const std::string&str);
  void Disappearance(Eris::Room*, const std::string&str);
  void Changed(const Eris::StringSet&);

  Eris::Lobby *_lobby;
  bool _initialised;
};  

} /* namespace Sear */
#endif /* CONSOLEOBJECT_H */

