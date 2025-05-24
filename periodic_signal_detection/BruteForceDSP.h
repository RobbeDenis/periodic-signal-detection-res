#pragma once
#include <vector>
#include <complex>

/* A collection of brute forced dsp functions */
namespace Brute
{
	/* Fourier Transform */

	std::vector<std::complex<double>> DiscreteFourierTransform(const std::vector<float>& input);
	std::vector<std::complex<double>> DiscreteFourierTransformOld(const std::vector<float>& input);
	std::vector<double> FourierCosineTransform(const std::vector<float>& input, bool makeAbs = true);
	std::vector<double> FourierSineTransform(const std::vector<float>& input, bool makeAbs = true);

	/* Filters */

	std::vector<double> GetPeakFreqThreshold(const std::vector<double>& input, double treshold, float samplerate, size_t N);
	std::vector<double> GetSimpleMeanFreq(const std::vector<double>& input, double tolerance);
}