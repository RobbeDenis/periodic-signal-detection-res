#include "FourierTransormTest.h"
#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"
#include "SignalGenerators.h"

FourierTransormTest::FourierTransormTest()
    : m_SampleRate{ 64.f }
    , m_BufferSize{ 1000 }
    , m_Source{ }
{
    m_Source.resize(m_BufferSize, 0.f);
    Generate::Sine(m_Source, 0.5f, 2.f, m_SampleRate);
    Generate::Sine(m_Source, 0.5f, 3.f, m_SampleRate);
}

FourierTransormTest::~FourierTransormTest()
{
    
}

void FourierTransormTest::Init()
{
}

void FourierTransormTest::Update()
{
    ImGui::Begin("FourierTransormTest");

    ImVec2 size{ -1, 0 };
    ImPlotFlags plotFlags{ };

    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Buffers", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotLine("Source", m_Source.data(), m_BufferSize);

        ImPlot::EndPlot();
    }

    ImGui::End();
}

void FourierTransormTest::Reset()
{
}
