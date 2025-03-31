#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define SAMPLE_RATE 48000
#define SINE_FREQUENCY 2000 // 1 kHz sine wave
#define PI 3.14159265358979323846

void generate_sine_wave(int32_t *buffer, size_t buffer_size) {
  static float phase = 0.0f; // Keep track of phase across function calls
  float phase_increment = 2.0f * PI * SINE_FREQUENCY /
                          SAMPLE_RATE; // Phase increment for each sample

  for (size_t i = 0; i < buffer_size; i++) {
    // Calculate the sine value for the current phase
    float sine_value = sinf(phase);

    // Convert the sine value to the 32-bit signed integer format
    // Sine wave values range from -1.0 to 1.0, so scale to the full range of a
    // 32-bit signed integer
    buffer[i] = (int32_t)(sine_value * 2147483647.0f); // Scale to INT32_MAX

    // Update the phase, wrapping around if it exceeds 2*PI
    phase += phase_increment;
    if (phase >= 2.0f * PI) {
      phase -= 2.0f * PI;
    }
  }
}
