#pragma once
#include <vector>

namespace Generate
{
	/* Add's a sine wave to the buffer */
	void Sine(std::vector<float>& buffer, float amp, float freq, float samplerate);
}