#include "Sound.h"
#include "Exception.h"
#include "Console.h"
#include "common/Log.h"

namespace Sear {

void Sound::init() {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    Log::writeLog(std::string("Error init SDL_AUDIO: ") + SDL_GetError(), Log::LOG_ERROR);
    throw Exception("Error initialising SDL Sound Subsystem!");
  }
#define MONO (1)
#define STEREO (2)
 if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, STEREO, 4096) != 0) {
    Log::writeLog(std::string("Error init SDL_mixer: ") + Mix_GetError(), Log::LOG_ERROR);
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
  while(!music_map.empty()) {
    Mix_Music *music = (music_map.begin())->second;
    if (music) Mix_FreeMusic(music);
    music_map.erase(music_map.begin());
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
  Log::writeLog(std::string("Mix_LoadWAV: ") + Mix_GetError(), Log::LOG_ERROR);
  return NULL;
}

Mix_Music *Sound::getMusic(const std::string &file_name) {
  Mix_Music *music = NULL;
  music = music_map[file_name];
  if (music) return music;
  music=Mix_LoadMUS(file_name.c_str());
  if (music) {
    music_map[file_name] = music;
    return music;
  }
  Log::writeLog(std::string("Mix_LoadMUS: ") + Mix_GetError(), Log::LOG_ERROR);
  return NULL;
}

void Sound::playSound(const std::string &file_name) {
  // load sample.wav in to sample
 Mix_Chunk *sample = getSample(file_name);
 Mix_PlayChannel(1, sample, 0);
}

void Sound::playSoundLoop(const std::string &file_name) {
  // load sample.wav in to sample
 Mix_Chunk *sample = getSample(file_name);
 Mix_PlayChannel(2, sample, -1);
}

void Sound::stopSoundLoop() {
  Mix_HaltChannel(2);
}

void Sound::playMusic(const std::string &file_name) {
  // load sample.wav in to sample
 Mix_Music *music = getMusic(file_name);
 Mix_PlayMusic(music, 0);
}

void Sound::stopMusic() {
  Mix_FadeOutMusic(100);
  Mix_RewindMusic();
  
}
void Sound::registerCommands(Console *console) {
  console->registerCommand(PLAY_SOUND, this);
  console->registerCommand(PLAY_SOUND_LOOP, this);
  console->registerCommand(STOP_SOUND_LOOP, this);
  console->registerCommand(PLAY_MUSIC, this);
  console->registerCommand(STOP_MUSIC, this);
}

void Sound::runCommand(const std::string &command, const std::string &args) {
  if (command == PLAY_SOUND) playSound(args);
  else if (command == PLAY_SOUND_LOOP) playSoundLoop(args);
  else if (command == STOP_SOUND_LOOP) stopSoundLoop();
  else if (command == PLAY_MUSIC) playMusic(args);
  else if (command == STOP_MUSIC) stopMusic();
}

} /* namespace Sear */