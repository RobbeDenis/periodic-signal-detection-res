#include "SignalDetectionWindow.h"
#include <numbers>
#include <algorithm>

void SDFunctions::PopulateBuffer(std::vector<float>& buffer, float samplerate, float period, bool printContents)
{
    using namespace std::numbers;

    std::cout << "\n\nPopulating the buffer...\n";

    const float amp{ 1.f };
    const float freq{ period };

    const float amp2{ 3.f };
    const float freq2{ period * 2.f };

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
    const int size{ static_cast<int>(foldedHistogram.size()) };
    const float width{ trailPeriod / size };

    for (float sample : foldedBuffer)
    {
        int index = std::min(static_cast<int>(sample / width), size - 1);
        foldedHistogram[index]++;
    }
}
