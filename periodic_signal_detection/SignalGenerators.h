#pragma once
#include <vector>

namespace Generate
{
	/* Add's a sine wave to the buffer */
	void Sine(std::vector<float>& buffer, float amp, float freq, float samplerate);

	/* Add's a cosine wave to the buffer */
	void Cosine(std::vector<float>& buffer, float amp, float freq, float samplerate);

	/* Add's random noise to the buffer */
	void RndNoise(std::vector<float>& buffer, float min, float max);
}