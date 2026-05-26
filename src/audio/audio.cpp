#include "audio.h"

Audio::Audio(bool is_headless) : headless(is_headless) {
  if (!headless) {
    SDL_AudioSpec audio_spec;
    audio_spec.freq = Chip8::AUDIO_SAMPLE_RATE;
    audio_spec.channels = 1;
    audio_spec.format = SDL_AUDIO_F32;

    SDL_AudioStream* raw_auido_stream =
        SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, nullptr, nullptr);

    if (!raw_auido_stream) {
      throw std::runtime_error("Error Initialising Audio Stream");
    }

    this->audio_stream = SmartAudioStream{raw_auido_stream};

    SDL_ResumeAudioStreamDevice(this->audio_stream.get());
  }
}

void Audio::start_beep() {
  int sample_chunk_size = Chip8::AUDIO_SAMPLE_RATE / Chip8::TIMER_HZ;
  std::vector<float> audio_buffer(sample_chunk_size);

  float phase_increment = (Chip8::AUDIO_FREQUENCY * 2.0f * M_PI) / 44100.0f;

  for (int i = 0; i < sample_chunk_size; i++) {
    audio_buffer[i] = (std::sin(this->phase) > 0.0f) ? Chip8::AUDIO_VOLUME : -Chip8::AUDIO_VOLUME;
    this->phase += phase_increment;
  }
  if (headless)
    return;

  SDL_PutAudioStreamData(this->audio_stream.get(), audio_buffer.data(),
                         audio_buffer.size() * sizeof(float));
}

void Audio::stop_beep() {
  if (headless)
    return;
  SDL_ClearAudioStream(this->audio_stream.get());
}
