#include "BruteForceDSP.h"
#include <numbers>

using namespace std::numbers;

std::vector<std::complex<double>> Brute::DiscreteFourierTransform(const std::vector<float>& input)
{
    const int N = static_cast<int>(input.size());
    const int K{ N / 2};
    std::complex<double> sum{ 0., 0. };

    std::vector<std::complex<double>> output;
    output.reserve(K);

    for (int k{ 0 }; k < K; ++k)
    {
        sum = std::complex<double>(0., 0.);
        for (int n{ 0 }; n < N; ++n)
        {
            double r = cos(2. * pi_v<double> *n * k / N);
            double z = -sin(2. * pi_v<double> *n * k / N);
            std::complex<double> w(r, z);
            sum += static_cast<double>(input[n]) * w;
        }

        output.push_back(sum);
    }

    return output;
}

std::vector<double> Brute::FourierCosineTransform(const std::vector<float>& input, bool makeAbs)
{
    const int N = static_cast<int>(input.size());
    const int K{ N / 2 };
    double sum{ 0. };

    std::vector<double> output;
    output.reserve(K);

    for (int k{ 0 }; k < K; ++k)
    {
        sum = 0.;
        for (int n{ 0 }; n < N; ++n)
        {
            sum += input[n] * cos(2. * pi_v<double> * n * k / N);
        }

        makeAbs? output.push_back(std::abs(sum)) : output.push_back(sum);
    }

    return output;
}

std::vector<double> Brute::FourierSineTransform(const std::vector<float>& input, bool makeAbs)
{
    const int N = static_cast<int>(input.size());
    const int K{ N / 2 };
    double sum{ 0. };

    std::vector<double> output;
    output.reserve(K);

    for (int k{ 0 }; k < K; ++k)
    {
        sum = 0.;
        for (int n{ 0 }; n < N; ++n)
        {
            sum += input[n] * sin(2. * pi_v<double> *n * k / N);
        }

        makeAbs ? output.push_back(std::abs(sum)) : output.push_back(sum);
    }

    return output;
}
