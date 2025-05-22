#include "SignalGenerators.h"
#include <numbers>
#include <random>

using namespace std::numbers;

float getRandomFloat(float min, float max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

void Generate::Sine(std::vector<float>& buffer, float amp, float freq, float samplerate)
{
	const float step{ 1.f / samplerate };
	for (int i{ 0 }; i < buffer.size(); ++i)
	{
		const float time{ static_cast<float>(i) * step };
		buffer[i] += amp * sinf(2 * pi_v<float> * freq * time);
	}
}

void Generate::Cosine(std::vector<float>& buffer, float amp, float freq, float samplerate)
{
	const float step{ 1.f / samplerate };
	for (int i{ 0 }; i < buffer.size(); ++i)
	{
		const float time{ static_cast<float>(i) * step };
		buffer[i] += amp * cosf(2 * pi_v<float> * freq * time);
	}
}

void Generate::RndNoise(std::vector<float>& buffer, float min, float max)
{
	for (int i{ 0 }; i < buffer.size(); ++i)
	{
		buffer[i] += getRandomFloat(min, max);
	}
}
