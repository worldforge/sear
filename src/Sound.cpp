#include "Sound.h"
#include "Exception.h"
#include "Console.h"

namespace Sear {

void Sound::init() {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    std::cerr << SDL_GetError() << std::endl << std::flush;
    throw Exception("Error initialising SDL Sound Subsystem!");
  }
#define MONO (1)
#define STEREO (2)
 if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, STEREO, 4096) != 0) {
    std::cerr << Mix_GetError() << std::endl << std::flush;
    throw Exception("Error initialising SDL_mixer!");
  }
}

void Sound::shutdown() {
  Mix_CloseAudio();
  while(!sound_map.empty()) {
    Mix_Chunk *sample = (sound_map.begin())->second;
    if (sample) Mix_FreeChunk(sample);
    sound_map.erase(sound_map.begin());
  }
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

Mix_Chunk *Sound::getSample(const std::string &file_name) {
  Mix_Chunk *sample = NULL;
  sample = sound_map[file_name];
  if (sample) return sample;
  sample=Mix_LoadWAV(file_name.c_str());
  if (sample) {
    sound_map[file_name] = sample;
    return sample;
  }
  printf("Mix_LoadWAV: %s\n", Mix_GetError());
  return NULL;
  
}

void Sound::playsound(const std::string &file_name) {
  // load sample.wav in to sample
 Mix_Chunk *sample = getSample(file_name);
 Mix_PlayChannel(-1, sample, 0);
}

void Sound::registerCommands(Console *console) {
  console->registerCommand(PLAY_SOUND, this);
}

void Sound::runCommand(const std::string &command, const std::string &args) {
  if (command == PLAY_SOUND) playsound(args);
}

} /* namespace Sear */
