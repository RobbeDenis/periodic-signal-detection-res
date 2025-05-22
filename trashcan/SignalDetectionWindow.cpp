#include "SignalDetectionWindow.h"
#include <random>

float getRandomFloat(float min, float max) 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

void SDFunctions::PopulateBuffer(std::vector<float>& buffer, float period, float secondsPerStep, bool printContents)
{
    using namespace std::numbers;

    const float baseFreq{ 1.f / period };

    const float amp{ 2.f };
    const float freq{ baseFreq };

    const float amp2{ 5.f };
    const float freq2{ baseFreq * 3.f};

    for (int i{ 0 }; i < buffer.size(); ++i)
    {
        float time{ i * secondsPerStep };
        float sample{ amp * std::sin(2 * pi_v<float> * freq * time) };
        float sample2{ amp2 * std::sin(2 * pi_v<float> * freq2 * time) };

        buffer[i] = sample + sample2;

        if (printContents)
        {
            std::cout << buffer[i] << ", ";
        }
    }
}

void SDFunctions::AddBufferNoise(std::vector<float>& buffer)
{
    for (float& buffer : buffer)
    {
        buffer += getRandomFloat(SDWindow::minRnd, SDWindow::maxRnd);
    }
}

void SDFunctions::PopulateBufferRnd(std::vector<float>& buffer)
{
    for (float& buffer : buffer)
    {
        buffer = getRandomFloat(SDWindow::minRnd, SDWindow::maxRnd);
    }
}

bool SDFunctions::ShiftPositive(std::vector<float>& buffer, float& shift, bool printContents)
{
    const float min{ *std::min_element(begin(buffer), end(buffer)) };
    const bool shifted{ min < 0.f };

    if (shifted)
    {
        shift = std::abs(min);

        for (float& sample : buffer)
        {
            sample += shift;

            if (printContents)
            {
                std::cout << sample << ", ";
            }
        }
    }

    return shifted;
}

void SDFunctions::FoldBuffer(std::vector<float>& buffer, std::vector<float>& foldedBuffer, float trailPeriod, bool printContents)
{
    for (int i{ 0 }; i < buffer.size(); ++i)
    {
        foldedBuffer[i] = buffer[i] - (static_cast<int>((buffer[i] / trailPeriod)) * trailPeriod);

        if (printContents)
        {
            std::cout << foldedBuffer[i] << ", ";
        }
    }
}

void SDFunctions::FillFoldedHistogram(std::vector<float>& foldedBuffer, std::vector<int>& foldedHistogram, float trailPeriod)
{
    const int size{ static_cast<int>(foldedHistogram.size()) };
    const float width{ std::max(std::numeric_limits<float>::epsilon(), trailPeriod / size) };

    for (int& bin : foldedHistogram)
        bin = 0;

    for (float sample : foldedBuffer)
    {
        int index = std::min(static_cast<int>(sample / width), size - 1);
        foldedHistogram[index]++;
    }
}

void SDFunctions::DeMeanFoldedHistogram(std::vector<int>& foldedHistogram, std::vector<float>& deMeanedHistogram)
{
    float mean{ 0 };
    for (int num : foldedHistogram)
    {
        mean += static_cast<float>(num);
    }

    mean /= foldedHistogram.size();

    for (int i{ 0 }; i < foldedHistogram.size(); ++i)
    {
        deMeanedHistogram[i] = foldedHistogram[i] - mean;
    }
}

void SDFunctions::AutoCorrelationFunction(std::vector<float>& deMeanedHistogram, std::vector<float>& acorHistogram, float bufferSize)
{
    size_t k{ deMeanedHistogram.size() };

    for (int j{ 0 }; j < k / 2; ++j)
    {
        acorHistogram[j] = 0.f;

        for (int i{ 0 }; i < k; ++i)
        {
            if(i + j < k)
                acorHistogram[j] += deMeanedHistogram[i] * deMeanedHistogram[i + j];
            else
                acorHistogram[j] += deMeanedHistogram[i] * deMeanedHistogram[i + j - k];
        }

        acorHistogram[j] /= bufferSize;
    }
}

float SDFunctions::CosineFourierTransform(std::vector<float>& acorHistogram)
{
    size_t k{ acorHistogram.size() };
    float sum{ 0.f };

    for (int j{ 0 }; j < k / 2; ++j)
    {
        sum += acorHistogram[j] * cos(((2 * std::numbers::pi_v<double>) / k) * j);
    }

    return sum;
}
