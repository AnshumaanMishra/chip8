#pragma once
#include "../common/constants.h"
#include "../common/shared_libs.h"
#include "SDL3/SDL_audio.h"

using SmartAudioStream = std::unique_ptr<SDL_AudioStream, void (*)(SDL_AudioStream*)>;

class Audio {
 private:
  SmartAudioStream audio_stream{nullptr, nullptr};

  float phase = 0.0f;

 public:
  Audio();
  void start_beep();
  void stop_beep();
};
