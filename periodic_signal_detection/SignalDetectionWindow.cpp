#include "SignalDetectionWindow.h"
#include <numbers>
#include <algorithm>
#include <random>

float getRandomFloat(float min, float max) 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

void SDFunctions::PopulateBuffer(std::vector<float>& buffer, float samplerate, float period, bool printContents)
{
    using namespace std::numbers;

    std::cout << "\n\nPopulating the buffer...\n";

    const float baseFreq{ 1.f / period };

    const float amp{ 1.f };
    const float freq{ baseFreq };

    const float amp2{ 3.f };
    const float freq2{ baseFreq * 2.f };

    for (int i{ 0 }; i < buffer.size(); ++i)
    {
        float time{ (i / samplerate) * pi_v<float> };
        float sample{ amp * std::sin(time * freq) };
        float sample2{ amp2 * std::sin(time * freq2) };

        buffer[i] = sample + sample2;

        if (printContents)
        {
            std::cout << buffer[i] << ", ";
        }
    }

    std::cout << "\nPopulating complete\n\n";
}

void SDFunctions::PopulateBufferRnd(std::vector<float>& buffer)
{
    for (float& buffer : buffer)
    {
        buffer = getRandomFloat(SDWindow::minRnd, SDWindow::maxRnd);
    }
}

void SDFunctions::ClearBuffers(std::vector<float>& buffer, std::vector<float>& foldedBuffer)
{
    for (int i{ 0 }; i < buffer.size(); ++i)
    {
        buffer[i] = 0.f;
        foldedBuffer[i] = 0.f;
    }
}

bool SDFunctions::ShiftPositive(std::vector<float>& buffer, float& shift, bool printContents)
{
    const float min{ *std::min_element(begin(buffer), end(buffer)) };
    const bool shifted{ min < 0.f };

    std::cout << "\n\nChecking for buffer shift\n";
    std::cout << "Min value in buffer: " << min << "\n";
    if (shifted)
    {
        std::cout << "Started buffer shift...\n";
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

    std::cout << "\nShifting complete\n\n";

    return shifted;
}

void SDFunctions::FoldBuffer(std::vector<float>& buffer, std::vector<float>& foldedBuffer, float trailPeriod, bool printContents)
{
    std::cout << "\n\nFolding the buffer...\n";
    for (int i{ 0 }; i < buffer.size(); ++i)
    {
        foldedBuffer[i] = buffer[i] - (static_cast<int>((buffer[i] / trailPeriod))) * trailPeriod;

        if (printContents)
        {
            std::cout << foldedBuffer[i] << ", ";
        }
    }
    std::cout << "\nFolding complete\n\n";
}

void SDFunctions::FillFoldedHistogram(std::vector<float>& foldedBuffer, std::vector<int>& foldedHistogram, float trailPeriod)
{
    std::cout << "\n\nCreating folded histogram...\n";
    const int size{ static_cast<int>(foldedHistogram.size()) };
    const float width{ trailPeriod / size };

    for (float sample : foldedBuffer)
    {
        int index = std::min(static_cast<int>(sample / width), size - 1);
        foldedHistogram[index]++;
    }
    std::cout << "\nCreating complete\n\n";
}

void SDFunctions::DeMeanFoldedHistogram(std::vector<int>& foldedHistogram, std::vector<float>& deMeanedHistogram)
{
    std::cout << "\n\nRemoving mean...\n";
    float mean{ 0 };
    for (int num : foldedHistogram)
    {
        mean += static_cast<float>(num);
    }

    mean /= foldedHistogram.size();
    std::cout << "Mean: " << mean << std::endl;

    for (int i{ 0 }; i < foldedHistogram.size(); ++i)
    {
        deMeanedHistogram[i] = foldedHistogram[i] - mean;
    }
    std::cout << "Mean removed\n";
}

void SDFunctions::AutoCorrelationFunction(std::vector<float>& deMeanedHistogram, std::vector<float>& acorHistogram, float bufferSize)
{
    size_t k{ deMeanedHistogram.size() };

    for (int j{ 0 }; j < k - 1; ++j)
    {
        if (j > k / 2.f)
            continue;

        for (int i{ 0 }; i < k - j; ++i)
        {
            if(i + j <= k)
                acorHistogram[j] += (deMeanedHistogram[i] * deMeanedHistogram[i + j]) / bufferSize;
            else
                acorHistogram[j] += (deMeanedHistogram[i] * (deMeanedHistogram[i + j] - k)) / bufferSize;
        }
    }
}
