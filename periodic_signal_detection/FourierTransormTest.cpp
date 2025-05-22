#include "FourierTransormTest.h"
#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include "SignalGenerators.h"
#include "BruteForceDSP.h"

FourierTransormTest::FourierTransormTest()
    : m_SampleRate{ 2000.f }
    , m_TestFrequency{ 105.58f }
    , m_BufferSize{ 20000 }
    , m_Source{ }
{
    m_Source.resize(m_BufferSize, 0.f);
    Generate::Sine(m_Source, 1.f, m_TestFrequency, m_SampleRate);
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

    if (ImGui::Button("Cosine T"))
    {
        m_CosineFT = Brute::GetCosineTransform(m_Source);
    }
    ImGui::SameLine();
    if (ImGui::Button("Sine T"))
    {
        m_CosineFT = Brute::GetSineTransform(m_Source);
    }

    ImGui::SameLine();
    float window_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(window_width);
    ImGui::BeginGroup();
    ImGui::Text("Samplerate: %.0f", m_SampleRate);
    ImGui::Text("Buffer size: %d", m_BufferSize);
    ImGui::Text("Signal frequency: %.2f", m_TestFrequency);
    ImGui::Text("Signal period: %.7f", 1.f / m_TestFrequency);
    ImGui::EndGroup();

    ImVec2 size{ -1, 0 };
    ImPlotFlags plotFlags{ };

    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Buffers", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotLine("Source", m_Source.data(), static_cast<int>(m_Source.size()));
        ImPlot::PlotLine("Cosine FT", m_CosineFT.data(), static_cast<int>(m_CosineFT.size()));

        ImPlot::EndPlot();
    }


    auto peak = std::max_element(begin(m_CosineFT), end(m_CosineFT));
    auto dip = std::min_element(begin(m_CosineFT), end(m_CosineFT));

    if (peak != end(m_CosineFT))
    {
        size_t index_peak = std::distance(begin(m_CosineFT), peak);
        float freq_peak = m_SampleRate * index_peak / m_BufferSize;
        float period_peak = 1.f / freq_peak;

        size_t index_dip = std::distance(begin(m_CosineFT), dip);
        float freq_dip = m_SampleRate * index_dip / m_BufferSize;
        float period_dip = 1.f / freq_dip;

        ImGui::BeginGroup();
        ImGui::Text("Peak", index_peak);
        ImGui::Text("index: %d", index_peak);
        ImGui::Text("value: %.4f", *peak);
        ImGui::Text("freq estimate: %.7f", freq_peak);
        ImGui::Text("period estimate: %.7f", period_peak);
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::Text("Dip");
        ImGui::Text("index: %d", index_dip);
        ImGui::Text("value: %.4f", *dip);
        ImGui::Text("freq estimate: %.7f", freq_dip);
        ImGui::Text("period estimate: %.7f", period_dip);
        ImGui::EndGroup();
    }

    ImGui::End();
}

void FourierTransormTest::Reset()
{
}
