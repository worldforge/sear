#include "Sound.h"
#include "Exception.h"
#include "Console.h"

namespace Sear {

Uint8 *Sound::decoded_ptr = NULL;
Uint32 Sound::decoded_bytes = 0;
int Sound::predecode = 0;
int Sound::looping = 0;
bool Sound::done_flag = false;
  
void Sound::init() {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    std::cerr << SDL_GetError() << std::endl << std::flush;
    throw Exception("Error initialising SDL Sound Subsystem!");
  }
  if (!Sound_Init()) {
    std::cerr << Sound_GetError() << std::endl << std::flush;
    throw Exception("Error initialising SDL Sound Library!");
  }
}

void Sound::shutdown() {
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void Sound::playsound(const std::string &file_name) {
  Uint32 audio_buffersize = DEFAULT_AUDIOBUF;
  Uint32 decode_buffersize = DEFAULT_DECODEBUF;
  SDL_AudioSpec sdl_desired; 
  static Sound_Sample *sample = NULL;
  if (sample) {
    finish_playback(sample);
    sample = NULL;
    done_flag = false;
  }
  sample = Sound_NewSampleFromFile(file_name.c_str(), NULL, decode_buffersize);
  if (!sample) {
    throw Exception("Error loading file");
  }
  sdl_desired.freq = sample->actual.rate;
  sdl_desired.format = sample->actual.format;
  sdl_desired.channels = sample->actual.channels;

  sdl_desired.samples = audio_buffersize;
  sdl_desired.callback = audio_callback;
  sdl_desired.userdata = sample;
  if (SDL_OpenAudio(&sdl_desired, NULL) < 0) {
    throw Exception("Error opening file");
  }
  SDL_PauseAudio(0);
}

void Sound::finish_playback(Sound_Sample *sample) {
  SDL_PauseAudio(1);
  if (sample->flags & SOUND_SAMPLEFLAG_ERROR) {
    fprintf(stderr, "Error in decoding sound file!\n reason: [%s].\n", Sound_GetError());
  } /* if */
  SDL_CloseAudio();  /* reopen with next sample's format if possible */
  Sound_FreeSample(sample);
  done_flag = true;
}

void Sound::audio_callback(void *userdata, Uint8 *stream, int len) {
  Sound_Sample *sample = (Sound_Sample *) userdata;
  int bw = 0; /* bytes written to stream this time through the callback */
  while (bw < len) {
    int cpysize;  /* bytes to copy on this iteration of the loop. */
    if (!read_more_data(sample)) { /* read more data, if needed. */
       /* ...there isn't any more data to read! */
       memset(stream + bw, '\0', len - bw);
       done_flag = 1;
  //     finish_playback(sample);
       return;
    } /* if */
    /* decoded_bytes and decoder_ptr are updated as necessary... */
    cpysize = len - bw;
    if (cpysize > decoded_bytes) cpysize = decoded_bytes;
    if (cpysize > 0) {
      memcpy(stream + bw, decoded_ptr, cpysize);
      bw += cpysize;
      decoded_ptr += cpysize;
      decoded_bytes -= cpysize;
    } /* if */
  } /* while */
} /* audio_callback */


int Sound::read_more_data(Sound_Sample *sample) {
  if (done_flag)              /* probably a sigint; stop trying to read. */
  decoded_bytes = 0;
  if (decoded_bytes > 0)      /* don't need more data; just return. */
  return(decoded_bytes);  
  /* need more. See if there's more to be read... */
  if (!(sample->flags & (SOUND_SAMPLEFLAG_ERROR | SOUND_SAMPLEFLAG_EOF))) {
    decoded_bytes = Sound_Decode(sample);
    decoded_ptr = (Uint8*)sample->buffer;
    return(read_more_data(sample));  /* handle loops conditions. */
  } /* if */
  /* No more to be read from stream, but we may want to loop the sample. */
  if (!looping) return(0);
  /* we just need to point predecoded samples to the start of the buffer. */
  Sound_Rewind(sample);  /* error is checked in recursion. */
  return(read_more_data(sample));
} /* read_more_data */

void Sound::registerCommands(Console *console) {
  console->registerCommand(PLAY_SOUND, this);
}

void Sound::runCommand(const std::string &command, const std::string &args) {
  if (command == PLAY_SOUND) playsound(args);
}

} /* namespace Sear */
