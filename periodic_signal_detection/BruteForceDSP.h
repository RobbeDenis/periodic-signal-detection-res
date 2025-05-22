#pragma once
#include <vector>

namespace Brute
{
	std::vector<double> GetCosineTransform(const std::vector<float>& input, bool makeAbs = true);
	std::vector<double> GetSineTransform(const std::vector<float>& input, bool makeAbs = true);
}