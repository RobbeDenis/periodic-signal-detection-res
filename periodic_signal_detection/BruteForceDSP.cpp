#include "BruteForceDSP.h"
#include <numbers>
#include <algorithm>
#include <iostream>

using namespace std::numbers;

std::vector<std::complex<double>> Brute::DiscreteFourierTransformLiterals(const std::vector<float>& input)
{
    if (input.empty())
    {
        std::cout << "DiscreteFourierTransform: input was empty\n";
        return std::vector<std::complex<double>>();
    }

    using namespace std::complex_literals;

    const int N = static_cast<int>(input.size());
    const int K{ N / 2 };
    std::complex<double> sum{ 0., 0. };
    std::complex<double> z{ -1i * 2. * pi_v<double> };
    std::complex<double> w;

    std::vector<std::complex<double>> output;
    output.reserve(K);

    for (int k{ 0 }; k < K; ++k)
    {
        sum = std::complex<double>(0., 0.);
        w = static_cast<double>(k) / static_cast<double>(N) * z;
        for (int n{ 0 }; n < N; ++n)
        {
            sum += static_cast<double>(input[n]) * std::exp(w * static_cast<double>(n));
        }
        output.push_back(sum);
    }

    return output;
}

std::vector<std::complex<double>> Brute::Opti_DiscreteFourierTransformLiterals(const std::vector<float>& input)
{
    if (input.empty())
    {
        std::cout << "DiscreteFourierTransform: input was empty\n";
        return std::vector<std::complex<double>>();
    }

    using namespace std::complex_literals;

    const std::complex<double> z{ 2. * pi_v<double> * -1i };
    const int N = static_cast<int>(input.size());
    const int K{ N / 2 };
    double temp{ 0. };
    std::complex<double> sum{ 0., 0. };
    std::complex<double> w{ 0., 0. };

    std::vector<std::complex<double>> output;
    output.reserve(K);

    for (int k{ 0 }; k < K; ++k)
    {
        sum = 0.;
        temp = static_cast<double>(k) / static_cast<double>(N) * z.imag();

        for (int n{ 0 }; n < N; ++n)
        {
            w.imag(static_cast<double>(n) * temp);
            sum += static_cast<double>(input[n]) * std::exp(w);
        }
        output.push_back(sum);
    }
    return output;
}

std::vector<std::complex<double>> Brute::DiscreteFourierTransform(const std::vector<float>& input)
{
    if (input.empty())
    {
        std::cout << "DiscreteFourierTransformOld: input was empty\n";
        return std::vector<std::complex<double>>();
    }

    const int N = static_cast<int>(input.size());
    const int K{ N / 2};
    const double topi{ 2. * pi_v<double> };
    std::complex<double> sum;
    std::complex<double> w;

    std::vector<std::complex<double>> output;
    output.reserve(K);

    for (int k{ 0 }; k < K; ++k)
    {
        sum = std::complex<double>(0., 0.);
        double a{ static_cast<double>(k) / static_cast<double>(N) * topi };
        for (int n{ 0 }; n < N; ++n)
        {
            w = std::complex<double>(cos(a * static_cast<double>(n)), -sin(a * static_cast<double>(n)));
            sum += static_cast<double>(input[n]) * w;
        }

        output.push_back(sum);
    }

    return output;
}

std::vector<std::complex<double>> Brute::Opti_DiscreteFourierTransform(const std::vector<float>& input)
{
    if (input.empty())
    {
        std::cout << "DiscreteFourierTransformOld: input was empty\n";
        return std::vector<std::complex<double>>();
    }

    const int N = static_cast<int>(input.size());
    const int K{ N / 2 };
    const double topi{ 2. * pi_v<double> };
    double freq;
    std::complex<double> sum;

    std::vector<std::complex<double>> output;
    output.reserve(K);

    for (int k{ 0 }; k < K; ++k)
    {
        sum = std::complex<double>(0., 0.);
        double a{ static_cast<double>(k) / static_cast<double>(N) * topi };
        for (int n{ 0 }; n < N; ++n)
        {
            freq = a * static_cast<double>(n);
            sum += std::complex<double>(static_cast<double>(input[n]) * cos(freq), static_cast<double>(input[n]) * -sin(freq));
        }

        output.push_back(sum);
    }

    return output;
}

std::vector<double> Brute::FourierCosineTransform(const std::vector<float>& input, bool makeAbs)
{
    if (input.empty())
    {
        std::cout << "FourierCosineTransform: input was empty\n";
        return std::vector<double>();
    }

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
    if (input.empty())
    {
        std::cout << "FourierSineTransform: input was empty\n";
        return std::vector<double>();
    }

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

std::vector<double> Brute::GetPeakFreqThreshold(const std::vector<double>& input, double treshold, float samplerate, size_t N)
{
    std::vector<double> output{ };

    if (input.empty())
    {
        std::cout << "GetPeakFreqThreshold: input was empty\n";
        return output;
    }

    for (size_t k{ 0 }; k < input.size(); ++k)
    {
        if (input[k] > treshold)
        {
            output.push_back(k * samplerate / N);
        }
    }

    std::sort(output.begin(), output.end());
    return output;
}

std::vector<double> Brute::GetSimpleMeanFreq(const std::vector<double>& input, double tolerance)
{
    std::vector<double> output;

    if (input.empty())
    {
        std::cout << "GetSimpleMeanFreq: input was empty\n";
        return output;
    }

    double sum{ input[0] };
    int count{ 1 };

    for (size_t i{ 1 }; i < input.size(); ++i)
    {
        if (std::abs(input[i] - input[i - 1]) <= tolerance)
        {
            sum += input[i];
            ++count;
        }
        else
        {
            output.push_back(sum / count);
            sum = input[i];
            count = 1;
        }
    }

    output.push_back(sum / count);

    return output;
}
