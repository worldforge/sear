// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Sound.h,v 1.8 2004-05-23 13:32:29 jmt Exp $

#ifndef SEAR_SOUND_H
#define SEAR_SOUND_H 1

#include <string>
#include <map>

#include "interfaces/ConsoleObject.h"

#ifdef __APPLE__
    #include <SDL_mixer/SDL_mixer.h>
#else
    #include <SDL/SDL_mixer.h>
#endif
    
namespace Sear {

class Console;
	
class Sound : public ConsoleObject {
public:
  Sound();
  ~Sound();
  void init();
  void shutdown();
  Mix_Chunk *getSample(const std::string &file_name);
  Mix_Music *getMusic(const std::string &file_name);

  void playSound(const std::string &);
  void playSoundLoop(const std::string &);
  void stopSoundLoop();
  
  void playMusic(const std::string &);
  void stopMusic();
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
private:
 std::map<std::string, Mix_Chunk*> sound_map;
  std::map<std::string, Mix_Music*> music_map;
  bool _initialised;
};
  
} /* namespace Sear */

#endif /* SEAR_SOUND_H */

