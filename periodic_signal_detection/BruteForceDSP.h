#pragma once
#include <vector>
#include <complex>

namespace Brute
{
	/* K = N / 2 */
	std::vector<std::complex<double>> DiscreteFourierTransform(const std::vector<float>& input);

	/* K = N / 2 */
	std::vector<double> FourierCosineTransform(const std::vector<float>& input, bool makeAbs = true);

	/* K = N / 2 */
	std::vector<double> FourierSineTransform(const std::vector<float>& input, bool makeAbs = true);
}