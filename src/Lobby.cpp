// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Lobby.cpp,v 1.8 2002-12-23 17:29:35 simon Exp $

#include "Lobby.h"
#include <iostream>
#include "Console.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

Lobby::Lobby() :
  _lobby(NULL),
  _initialised(false)
{}

Lobby::~Lobby() {
  if (_initialised) shutdown();
}
	
void Lobby::init(Eris::Lobby *lobby) {
  if (_initialised) shutdown();
  _lobby = lobby;
  _lobby->PrivateTalk.connect(SigC::slot(*this, &Lobby::privateChat));
  _lobby->Entered.connect(SigC::slot(*this, &Lobby::Entered));
  _lobby->Talk.connect(SigC::slot(*this, &Lobby::Talk));
  _lobby->Emote.connect(SigC::slot(*this, &Lobby::Emote));
  _lobby->Appearance.connect(SigC::slot(*this, &Lobby::Appearance));
  _lobby->Disappearance.connect(SigC::slot(*this, &Lobby::Disappearance));
  _lobby->Changed.connect(SigC::slot(*this, &Lobby::Changed));
  _initialised = true;
}

void Lobby::say(const std::string &speech) {
  _lobby->say(speech);
}

void Lobby::emote(const std::string &speech) {
  _lobby->emote(speech);
}
void Lobby::sayPrivate(const std::string &speech, const std::string &to) {
  
}

void Lobby::shutdown() {
  _initialised = false;
}

void Lobby::render() {

}


void Lobby::privateChat(const std::string &src, const std::string &msg) {
  std::cout << src << ": " << msg << std::endl;
}

void Lobby::Entered(Eris::Room *room) {
  std::cout << "Entered room: " << room->getName() << std::endl;
}

void Lobby::Talk(Eris::Room *room, const std::string &who, const std::string &speech) {
  std::cout << "Talk: " << room->getName() << ": " << who << ": " << speech << std::endl;
}

void Lobby::Emote(Eris::Room *room, const std::string&who, const std::string&emote) {
  std::cout << "Emote: " << room->getName() << ": " << who << ": " << emote << std::endl;
}

void Lobby::Appearance(Eris::Room*room, const std::string&str) {
  std::cout << "Appearance: " << room->getName() << ": " << str <<  std::endl;
}

void Lobby::Disappearance(Eris::Room* room, const std::string&str) {
  std::cout << "Disappearance: " << room->getName() << ": " << str <<  std::endl;
}

void Lobby::Changed(const Eris::StringSet &ss) {
  std::cout << "Changed" << std::endl;
  for (Eris::StringSet::const_iterator I = ss.begin(); I != ss.end(); ++I) {
    std::cout << *I << std::endl;
  }
}

void Lobby::registerCommands(Console *console) {
  console->registerCommand(JOIN_ROOM, this);
  console->registerCommand(LEAVE_ROOM, this);
  console->registerCommand(LIST_ROOMS, this);
  console->registerCommand(TALK, this);
  console->registerCommand(EMOTE, this);
}

void Lobby::runCommand(const std::string &command, const std::string &args) {
  if (command == JOIN_ROOM) {
  }
  else if (command == LEAVE_ROOM) {
  }
  else if (command == LIST_ROOMS) {
    Eris::StringList sl = _lobby->getRooms();
    std::cout << "listing room"<< std::endl;
    for (Eris::StringList::const_iterator I = sl.begin(); I != sl.end(); ++I) {
      std::cout << *I << std::endl;
    }
  }
  else if (command == TALK) {
    say(args);	  
  }
  else if (command == EMOTE) {
    emote(args);
  }

}

} /* namespace Sear */
