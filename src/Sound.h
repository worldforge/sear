// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef SOUND_H
#define SOUND_H 1

#include <string>
#include <map>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "ConsoleObject.h"
    
namespace Sear {

class Console;
	
class Sound : public ConsoleObject {
public:
  Sound() {}
  ~Sound() {}
  void init();
  void shutdown();
  void playsound(const std::string &);
  Mix_Chunk *getSample(const std::string &file_name);

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
private:
  static const char * const PLAY_SOUND = "play_sound";
  std::map<std::string, Mix_Chunk*> sound_map;

};
  
} /* namespace Sear */

#endif /* SOUND_H */

