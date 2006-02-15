// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Sound.h,v 1.10 2006-02-15 12:44:24 simon Exp $

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
  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void playSound(const std::string &);
  void playSoundLoop(const std::string &);
  void stopSoundLoop();
  
  void playMusic(const std::string &);
  void stopMusic();
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
private:

  Mix_Chunk *getSample(const std::string &file_name);
  Mix_Music *getMusic(const std::string &file_name);

  std::map<std::string, Mix_Chunk*> m_sound_map;
  std::map<std::string, Mix_Music*> m_music_map;
  bool m_initialised;
};
  
} /* namespace Sear */

#endif /* SEAR_SOUND_H */

