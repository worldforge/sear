// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef SOUND_H
#define SOUND_H 1

#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_sound.h>

#include "ConsoleObject.h"
    
#define DEFAULT_DECODEBUF 16384
#define DEFAULT_AUDIOBUF  4096

#define PLAYSOUND_VER_MAJOR  0
#define PLAYSOUND_VER_MINOR  1
#define PLAYSOUND_VER_PATCH  5

namespace Sear {

class Console;
	
class Sound : public ConsoleObject {
public:
  Sound() {
//    decoded_ptr = NULL;
//    decoded_bytes = 0;
//    predecode = 0;
//    looping = 0;
  }
  ~Sound() {}
  void init();
  void shutdown();
  void playsound(const std::string &);

  static void audio_callback(void *userdata, Uint8 *stream, int len);
  static int read_more_data(Sound_Sample *sample);
  static void finish_playback(Sound_Sample *sample);

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
private:
  static Uint8 *decoded_ptr;
  static Uint32 decoded_bytes;
  static int predecode;
  static int looping;
  static bool done_flag;
  static const char * const PLAY_SOUND = "play_sound";

};
  
} /* namespace Sear */

#endif /* SOUND_H */

