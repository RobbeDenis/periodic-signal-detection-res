#include "BruteForceDSP.h"
#include <numbers>

using namespace std::numbers;

std::vector<double> Brute::GetCosineTransform(const std::vector<float>& input, bool makeAbs)
{
    const int N = static_cast<int>(input.size());
    const int K{ N };

    std::vector<double> output;
    output.reserve(K);

    for (int k{ 0 }; k < K / 2; ++k)
    {
        double sum{ 0. };
        for (int n{ 0 }; n < N; ++n)
        {
            sum += input[n] * cos(2. * pi_v<double> * n * k / N);
        }

        makeAbs? output.push_back(std::abs(sum)) : output.push_back(sum);
    }

    return output;
}

std::vector<double> Brute::GetSineTransform(const std::vector<float>& input, bool makeAbs)
{
    const int N = static_cast<int>(input.size());
    const int K{ N };

    std::vector<double> output;
    output.reserve(K);

    for (int k{ 0 }; k < K / 2; ++k)
    {
        double sum{ 0. };
        for (int n{ 0 }; n < N; ++n)
        {
            sum += input[n] * sin(2. * pi_v<double> *n * k / N);
        }

        makeAbs ? output.push_back(std::abs(sum)) : output.push_back(sum);
    }

    return output;
}
