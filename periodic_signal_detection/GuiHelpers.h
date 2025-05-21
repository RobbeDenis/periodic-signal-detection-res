#pragma once
#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

static void DrawPlotLine(const char* label, float value, float thickness = 1.f, const ImColor& color = { 1.f, 0.f, 0.f },  bool Xaxis = true)
{
    if (ImPlot::BeginItem(label))
    {
        const auto& limits = ImPlot::GetPlotLimits();
        if (Xaxis)
        {
            auto p1 = ImPlot::PlotToPixels(limits.X.Min, value);
            auto p2 = ImPlot::PlotToPixels(limits.X.Max, value);
            ImPlot::PushPlotClipRect();
            ImPlot::GetPlotDrawList()->AddLine(p1, p2, color, thickness);
            ImPlot::PopPlotClipRect();
            ImPlot::EndItem();
        }
    }
}