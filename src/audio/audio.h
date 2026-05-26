#pragma once
#include "../common/constants.h"
#include "../common/shared_libs.h"
#include "SDL3/SDL_audio.h"
struct SDL_AudioStreamDeleter {
  void operator()(SDL_AudioStream* stream) const {
    if (stream != nullptr) {
      SDL_DestroyAudioStream(stream);
    }
  }
};

using SmartAudioStream = std::unique_ptr<SDL_AudioStream, SDL_AudioStreamDeleter>;

class Audio {
 private:
  SmartAudioStream audio_stream{nullptr};
  bool headless{false};

  float phase = 0.0f;

 public:
  explicit Audio(bool is_headless = false);
  void start_beep();
  void stop_beep();
};
