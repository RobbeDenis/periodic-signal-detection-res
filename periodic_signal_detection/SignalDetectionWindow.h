#pragma once
#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"
#include "GuiHelpers.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>
#include <numbers>

namespace SDWindow
{
    /* data declarations */
    enum BufferState : unsigned char
    {
        BS_Empty,
        BS_Populated,
        BS_Positive,
        BS_Folded,
        BS_DeMeaned,
        BS_Autocorrelated,
    };

    /* init */
    static int bufferSize{ 10000 };
    static int numBins{ 100 };
    static float sourceSamplerate{ 32.f };
    static float period{ 1.f };
    static float secondsPerStep{ period / sourceSamplerate };
    static float trailPeriod{ 1.f };

    static int ctfScans{ 1000 };

    constexpr float minTau{ std::numeric_limits<float>::epsilon() };
    //const float minTau{ 80.f };
    const float maxTau{ 5.f };

    //const float slice{ (maxTau - minTau) / ctfScans * sourceSamplerate * pi_v<float> };
    //const float slice{ (maxTau - minTau) / ctfScans * sourceSamplerate };
    const float slice{ (maxTau - minTau) / ctfScans };

    static float maxRnd{ 0.4f };
    static float minRnd{ -0.3f };

    /* state */
    static BufferState bufferState{ BS_Empty };
    static float shift{ 0.f };
    static std::vector<int> foldedHistogram(numBins, 0);
    static std::vector<float> deMeanedHistogram(numBins, 0);
    static std::vector<float> acorHistogram(numBins, 0);
    static std::vector<float> cftHistogram(numBins, 0);

    /* buffers */
    static std::vector<float> source(bufferSize, 0.f);
    static std::vector<float> folded(bufferSize, 0.f);

    static std::vector<float> cftResults(ctfScans, 0.f);
}

namespace SDFunctions
{
    /* function declarations */
    void PopulateBuffer(std::vector<float>& buffer, float period, float secondsPerStep, bool printContents = false);
    void PopulateBufferRnd(std::vector<float>& buffer);
    void AddBufferNoise(std::vector<float>& buffer);

    bool ShiftPositive(std::vector<float>& buffer, float& shift, bool printContents = false);
    void FoldBuffer(std::vector<float>& buffer, std::vector<float>& foldedBuffer, float trailPeriod, bool printContents = false);
    void FillFoldedHistogram(std::vector<float>& foldedBuffer, std::vector<int>& foldedHistogram, float trailPeriod);
    void DeMeanFoldedHistogram(std::vector<int>& foldedHistogram, std::vector<float>& deMeanedHistogram);
    void AutoCorrelationFunction(std::vector<float>& deMeanedHistogram, std::vector<float>& acorHistogram, float bufferSize);
    float CosineFourierTransform(std::vector<float>& acorHistogram);
}

static void ShowSignalDetectionWindow()
{
    using namespace SDWindow;
    using namespace SDFunctions;
    using namespace std::numbers;

    bool disable{ false };

    ImGui::Begin("Periodic signal detection - Manual");

    /* Processing buttons */

    /* Populate */
    disable = BS_Empty > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("populate buffer"))
    {
        PopulateBuffer(source, period, secondsPerStep);
        //AddBufferNoise(source);

        //PopulateBufferRnd(source);
        bufferState = BS_Populated;
    }

    /* Clear */
    ImGui::SameLine();
    if (ImGui::Button("clear buffers"))
    {
        for (int i{ 0 }; i < bufferSize; ++i)
        {
            source[i] = 0.f;
            folded[i] = 0.f;
        }

        for (int i{ 0 }; i < numBins; ++i)
        {
            foldedHistogram[i] = 0;
            deMeanedHistogram[i] = 0.f;
            acorHistogram[i] = 0.f;
            cftHistogram[i] = 0.f;
        }

        bufferState = BS_Empty;
    }
    if (disable) ImGui::EndDisabled();

    /* Shift */
    disable = BS_Populated > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("shift+"))
    {
        if (ShiftPositive(source, shift))
        {
            bufferState = BS_Positive;

            float peak{ 0.f };
            int peakIdx{ 0 };

            for (int pass{ 0 }; pass < ctfScans; ++pass)
            {
                float tau{ slice * static_cast<float>(pass) };

                FoldBuffer(source, folded, tau);
                FillFoldedHistogram(folded, foldedHistogram, tau);
                DeMeanFoldedHistogram(foldedHistogram, deMeanedHistogram);
                AutoCorrelationFunction(deMeanedHistogram, acorHistogram, bufferSize);
                cftResults[pass] = CosineFourierTransform(acorHistogram) / sourceSamplerate;

                if (cftResults[pass] > peak && pass > 0)
                {
                    peak = cftResults[pass];
                    peakIdx = pass;
                }
            }

            std::cout << "Predicted index: " << static_cast<int>(period / slice) 
                << " cperiod: " << slice * static_cast<float>(period / slice) << " aperiod: " << period << std::endl;

            std::cout << "Index: " << peakIdx << " peak: " << peak << " period: " << slice * static_cast<float>(peakIdx) << std::endl;
        }
    }
    if (disable) ImGui::EndDisabled();



    /* Creating plots */
    ImVec2 size{ -1, 0 };
    ImPlotFlags plotFlags{ };

    /* Buffers */
    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Buffers", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotLine("Source", source.data(), bufferSize);
        ImPlot::PlotLine("Folded", folded.data(), bufferSize);

        ImPlot::EndPlot();
    }

    /* Meta data */
    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Meta data", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
        ImDrawList* draw_list = ImPlot::GetPlotDrawList();
        int* foldedData{ foldedHistogram.data() };

        ImPlot::PlotBars("Fold distr", foldedData, numBins);
        ImPlot::PlotLine("Fold distr line", foldedData, numBins);

        ImPlot::PlotBars("De-mean distr", deMeanedHistogram.data(), numBins);
        ImPlot::PlotLine("De-mean distr line", deMeanedHistogram.data(), numBins);

        ImPlot::PlotBars("Acor distr", acorHistogram.data(), numBins);
        ImPlot::PlotLine("Acor distr line", acorHistogram.data(), numBins);

        ImPlot::EndPlot();
    }

    /* Result */
    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Result", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotBars("Result", cftResults.data(), ctfScans);

        ImPlot::EndPlot();
    }

    ImGui::End();
}

static void ShowSignalDetectionWindowManual()
{
    using namespace SDWindow;
    using namespace SDFunctions;

    bool disable{ false };

    ImGui::Begin("Periodic signal detection - Manual");

    /* Processing buttons */

    /* Populate */
    disable = BS_Empty > bufferState;
    if(disable) ImGui::BeginDisabled();
    if (ImGui::Button("populate buffer"))
    {
        PopulateBuffer(source, sourceSamplerate, period);
        //PopulateBufferRnd(source);
        bufferState = BS_Populated;
    }

    /* Clear */
    ImGui::SameLine();
    if (ImGui::Button("clear buffers"))
    {
        for (int i{ 0 }; i < bufferSize; ++i)
        {
            source[i] = 0.f;
            folded[i] = 0.f;
        }

        for (int i{ 0 }; i < numBins; ++i)
        {
            foldedHistogram[i] = 0;
            deMeanedHistogram[i] = 0.f;
            acorHistogram[i] = 0.f;
            cftHistogram[i] = 0.f;
        }

        bufferState = BS_Empty;
    }
    if(disable) ImGui::EndDisabled();

    /* Clear */
    disable = BS_Populated > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("shift+"))
    {
        if (ShiftPositive(source, shift))
        {
            bufferState = BS_Positive;
        }
    }
    if (disable) ImGui::EndDisabled();

    /* Folding */
    ImGui::SameLine();
    disable = BS_Positive > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("fold"))
    {
        FoldBuffer(source, folded, trailPeriod);
        FillFoldedHistogram(folded, foldedHistogram, trailPeriod);
        bufferState = BS_Folded;
    }
    if (disable) ImGui::EndDisabled();

    /* Mean removal */
    ImGui::SameLine();
    disable = BS_Folded > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("de-mean"))
    {
        DeMeanFoldedHistogram(foldedHistogram, deMeanedHistogram);
        bufferState = BS_DeMeaned;
    }
    if (disable) ImGui::EndDisabled();

    /* Autocorrelation */
    ImGui::SameLine();
    disable = BS_DeMeaned > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("acor"))
    {
        AutoCorrelationFunction(deMeanedHistogram, acorHistogram, bufferSize);
        CosineFourierTransform(acorHistogram);
        bufferState = BS_Autocorrelated;
    }
    if (disable) ImGui::EndDisabled();

    
    /* Creating plots */
    ImVec2 size{ -1, 0 };
    ImPlotFlags plotFlags{ };

    /* Buffers */
    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Buffers", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotLine("Source", source.data(), bufferSize);
        ImPlot::PlotLine("Folded", folded.data(), bufferSize);

        //ImPlot::PlotInfLines("Trail period", &trailPeriod, 1, ImPlotInfLinesFlags_Horizontal, 0, 2);

        ImPlot::EndPlot();
    }

    /* Meta data */
    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Meta data", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
        ImDrawList* draw_list = ImPlot::GetPlotDrawList();
        int* foldedData{ foldedHistogram.data() };

        ImPlot::PlotBars("Fold distr", foldedData, numBins);
        ImPlot::PlotLine("Fold distr line", foldedData, numBins);

        ImPlot::PlotBars("De-mean distr", deMeanedHistogram.data(), numBins);
        ImPlot::PlotLine("De-mean distr line", deMeanedHistogram.data(), numBins);

        ImPlot::PlotBars("Acor distr", acorHistogram.data(), numBins);
        ImPlot::PlotLine("Acor distr line", acorHistogram.data(), numBins);

        ImPlot::EndPlot();
    }

    ImGui::End();
}

