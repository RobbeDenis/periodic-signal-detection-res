#pragma once
#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <complex>

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
    static int bufferSize{ 5000 };
    static int numBins{ 100 };
    static float samplerate{ 64.f };
    static float period{ 0.1f };
    static float trailPeriod{ 1.8f };

    static float maxRnd{ 20.f };
    static float minRnd{ -20.f };

    /* state */
    static BufferState bufferState{ BS_Empty };
    static float shift{ 0.f };
    static std::vector<int> foldedHistogram(numBins, 0);
    static std::vector<float> deMeanedHistogram(numBins, 0);
    static std::vector<float> acorHistogram(numBins, 0);

    /* buffers */
    static std::vector<float> source(bufferSize, 0.f);
    static std::vector<float> folded(bufferSize, 0.f);
}

namespace SDFunctions
{
    /* function declarations */
    void PopulateBuffer(std::vector<float>& buffer, float samplerate, float period, bool printContents = false);
    void PopulateBufferRnd(std::vector<float>& buffer);

    void ClearBuffers(std::vector<float>& buffer, std::vector<float>& foldedBuffer);
    bool ShiftPositive(std::vector<float>& buffer, float& shift, bool printContents = false);
    void FoldBuffer(std::vector<float>& buffer, std::vector<float>& foldedBuffer, float trailPeriod, bool printContents = false);
    void FillFoldedHistogram(std::vector<float>& foldedBuffer, std::vector<int>& foldedHistogram, float trailPeriod);
    void DeMeanFoldedHistogram(std::vector<int>& foldedHistogram, std::vector<float>& deMeanedHistogram);
    void AutoCorrelationFunction(std::vector<float>& deMeanedHistogram, std::vector<float>& acorHistogram, float bufferSize);
}

static void ShowSignalDetectionWindow()
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
        //PopulateBuffer(source, samplerate, period);
        PopulateBufferRnd(source);
        bufferState = BS_Populated;
    }

    /* Clear */
    ImGui::SameLine();
    if (ImGui::Button("clear buffers"))
    {
        ClearBuffers(source, folded);
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

