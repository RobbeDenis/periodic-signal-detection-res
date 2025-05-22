#include "SignalGenerators.h"
#include <numbers>

using namespace std::numbers;

void Generate::Sine(std::vector<float>& buffer, float amp, float freq, float samplerate)
{
	const float step{ 1.f / samplerate };
	for (int i{ 0 }; i < buffer.size(); ++i)
	{
		const float time{ static_cast<float>(i) * step };
		buffer[i] += amp * sinf(2 * pi_v<float> * freq * time);
	}
}
