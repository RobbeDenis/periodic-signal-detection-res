#pragma once
#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include <cmath>
#include <vector>
#include <iostream>

namespace SDWindow
{
    /* data declarations */
    enum BufferState : unsigned char
    {
        BS_Empty,
        BS_Populated,
        BS_Positive,
        BS_Folded,
    };

    /* init */
    static int bufferSize{ 3000 };
    static int numBins{ 25 };
    static float samplerate{ 32.f };
    static float period{ 1.f };
    static float trailPeriod{ 1.f };

    /* state */
    static BufferState bufferState{ BS_Empty };
    static float shift{ 0.f };
    static std::vector<int> foldedHistogram(numBins, 0);

    /* buffers */
    static std::vector<float> source(bufferSize, 0.f);
    static std::vector<float> folded(bufferSize, 0.f);
}

namespace SDFunctions
{
    /* function declarations */
    void PopulateBuffer(std::vector<float>& buffer, float samplerate, float period, bool printContents = false);
    void ClearBuffers(std::vector<float>& buffer, std::vector<float>& foldedBuffer);
    bool ShiftPositive(std::vector<float>& buffer, float& shift, bool printContents = false);
    void FoldBuffer(std::vector<float>& buffer, std::vector<float>& foldedBuffer, float trailPeriod, bool printContents = false);
    void FillFoldedHistogram(std::vector<float>& foldedBuffer, std::vector<int>& foldedHistogram, float trailPeriod);
}

static void ShowSignalDetectionWindow()
{
    using namespace SDWindow;
    using namespace SDFunctions;

    bool disable{ false };

    ImGui::Begin("Periodic signal detection");

    /* Processing buttons */
    disable = BS_Empty > bufferState;
    if(disable) ImGui::BeginDisabled();
    if (ImGui::Button("Populate buffer"))
    {
        PopulateBuffer(source, samplerate, period);
        bufferState = BS_Populated;
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear buffers"))
    {
        ClearBuffers(source, folded);
        bufferState = BS_Empty;
    }
    if(disable) ImGui::EndDisabled();

    disable = BS_Populated > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("Shift+"))
    {
        if (ShiftPositive(source, shift))
        {
            bufferState = BS_Positive;
        }
    }
    if (disable) ImGui::EndDisabled();

    ImGui::SameLine();

    disable = BS_Positive > bufferState;
    if (disable) ImGui::BeginDisabled();
    if (ImGui::Button("Fold"))
    {
        FoldBuffer(source, folded, trailPeriod);
        FillFoldedHistogram(folded, foldedHistogram, trailPeriod);
        bufferState = BS_Folded;
    }
    if (disable) ImGui::EndDisabled();

    
    /* Plots */
    ImVec2 size{ -1, 0 };
    ImPlotFlags plotFlags{ };

    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Buffers", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotLine("Source", source.data(), bufferSize);
        ImPlot::PlotLine("Folded", folded.data(), bufferSize);

        ImPlot::EndPlot();
    }

    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Meta data", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside | ImPlotLegendFlags_Horizontal);
        ImDrawList* draw_list = ImPlot::GetPlotDrawList();
        int* foldedData{ foldedHistogram.data() };

        ImPlot::PlotBars("Folded distr.", foldedData, numBins);
        ImPlot::PlotLine("Folded distr. line", foldedData, numBins);

        ImPlot::EndPlot();
    }

    ImGui::End();
}

