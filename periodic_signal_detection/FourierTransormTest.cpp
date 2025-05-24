#include "FourierTransormTest.h"
#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include <numbers>

#include "SignalGenerators.h"
#include "BruteForceDSP.h"

FourierTransormTest::FourierTransormTest()
    : m_SampleRate{ 512.f }
    , m_TestFrequency{ 8.9f }
    , m_BufferSize{ 5000 }
    , m_PeakTreshold{ 600. }
    , m_MeanTolerance{ 2. }
    , m_Source{ }
    , m_Output{ }
    , m_ComplexOutput{ }
{
    m_Source.resize(m_BufferSize, 0.f);
    m_Output.reserve(m_BufferSize / 2);

    Generate::Sine(m_Source, 1.f, m_TestFrequency, m_SampleRate);
    Generate::Sine(m_Source, 1.f, 54.f, m_SampleRate);
    Generate::Sine(m_Source, 1.f, 4.f, m_SampleRate);
    Generate::Sine(m_Source, 1.f, 164.f, m_SampleRate);
    Generate::Cosine(m_Source, 1.f, 47.41f, m_SampleRate);
    Generate::Cosine(m_Source, 1.f, 164.f, m_SampleRate);
    Generate::RndNoise(m_Source, -5.f, 5.f);
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

    if (ImGui::Button("Cosine FT"))
    {
        m_Output = Brute::FourierCosineTransform(m_Source);
        m_OutputFreq = Brute::GetPeakFreqThreshold(m_Output, m_PeakTreshold, m_SampleRate, m_BufferSize);
        m_OutputFreq = Brute::GetSimpleMeanFreq(m_OutputFreq, m_MeanTolerance);
    }
    ImGui::SameLine();
    if (ImGui::Button("Sine FT"))
    {
        m_Output = Brute::FourierSineTransform(m_Source);
        m_OutputFreq = Brute::GetPeakFreqThreshold(m_Output, m_PeakTreshold, m_SampleRate, m_BufferSize);
        m_OutputFreq = Brute::GetSimpleMeanFreq(m_OutputFreq, m_MeanTolerance);
    }
    ImGui::SameLine();
    if (ImGui::Button("DFT Old"))
    {
        m_ComplexOutput = Brute::DiscreteFourierTransformOld(m_Source);
        CopyComplexToOutput();
        m_OutputFreq = Brute::GetPeakFreqThreshold(m_Output, m_PeakTreshold, m_SampleRate, m_BufferSize);
        m_OutputFreq = Brute::GetSimpleMeanFreq(m_OutputFreq, m_MeanTolerance);
    }
    ImGui::SameLine();
    if (ImGui::Button("DFT"))
    {
        m_ComplexOutput = Brute::DiscreteFourierTransform(m_Source);
        CopyComplexToOutput();
        m_OutputFreq = Brute::GetPeakFreqThreshold(m_Output, m_PeakTreshold, m_SampleRate, m_BufferSize);
        m_OutputFreq = Brute::GetSimpleMeanFreq(m_OutputFreq, m_MeanTolerance);
    }

    ImGui::SameLine();
    float window_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(window_width);
    ImGui::BeginGroup();
    ImGui::Text("Samplerate: %.0f", m_SampleRate);
    ImGui::Text("Buffer size: %d", m_BufferSize);
    ImGui::Text("Peak treshold: %.f", m_PeakTreshold);
    ImGui::EndGroup();

    /* Plots */
    ImVec2 size{ -1, 0 };
    ImPlotFlags plotFlags{ };

    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Source", size, plotFlags))
    {
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotLine("Source", m_Source.data(), static_cast<int>(m_Source.size()));

        ImPlot::EndPlot();
    }

    ImGui::BeginGroup();
    InfoPeakAndDip();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    InfoPeakFrequencies();
    ImGui::EndGroup();

    ImGui::SameLine();
    ImPlot::SetNextAxisToFit(ImAxis_X1 | ImAxis_Y1);
    if (ImPlot::BeginPlot("Frequency domain", size, plotFlags))
    {
        ImPlot::PushColormap(ImPlotColormap_Spectral);
        ImPlot::SetupLegend(ImPlotLocation_NorthEast);
        ImPlot::PlotLine("Output", m_Output.data(), static_cast<int>(m_Output.size()));

        ImPlot::EndPlot();
    }

    ImGui::End();
}

void FourierTransormTest::Reset()
{
}

void FourierTransormTest::CopyComplexToOutput()
{
    m_Output.resize(m_ComplexOutput.size(), 0.);
    for (int i{ 0 }; i < m_ComplexOutput.size(); ++i)
    {
        m_Output[i] = std::abs(m_ComplexOutput[i]);
    }
}

void FourierTransormTest::InfoPeakFrequencies()
{
    ImGui::Text("Found frequencies");
    for (double freq : m_OutputFreq)
    {
        ImGui::Text("freq estimate: %.7f", freq);
    }
}

void FourierTransormTest::InfoPeakAndDip()
{
    auto peak = std::max_element(begin(m_Output), end(m_Output));
    auto dip = std::min_element(begin(m_Output), end(m_Output));

    if (peak == end(m_Output) || dip == end(m_Output))
        return;

    size_t index_peak = std::distance(begin(m_Output), peak);
    float freq_peak = m_SampleRate * index_peak / m_BufferSize;
    float period_peak = 1.f / freq_peak;

    size_t index_dip = std::distance(begin(m_Output), dip);
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
