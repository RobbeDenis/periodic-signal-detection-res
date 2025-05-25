#include "DFTSpeedTest.h"
#include "SignalGenerators.h"
#include "BruteForceDSP.h"

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include <chrono>
#include <iostream>

DFTSpeedTest::DFTSpeedTest()
	: DSPWindow()
    , m_SampleRate{ 2048.f }
    , m_TestFrequency{ 8.9f }
    , m_BufferSize{ 44100 }
    , m_PeakTreshold{ 600. }
    , m_MeanTolerance{ 2. }
    , m_Source{ }
    , m_Output{ }
    , m_ComplexOutput{ }
    , m_NumTests{ 1 }
{
    m_Source.resize(m_BufferSize, 0.f);
    Generate::Sine(m_Source, 1.f, m_TestFrequency, m_SampleRate);
    Generate::Sine(m_Source, 1.f, 880.f, m_SampleRate);
    Generate::Sine(m_Source, 1.f, 440.f, m_SampleRate);
}

DFTSpeedTest::~DFTSpeedTest()
{
}

void DFTSpeedTest::Init()
{
}

void DFTSpeedTest::Update()
{
    ImGui::Begin("Speed test");

    if (ImGui::Button("Test DFT"))
    {
        auto start = std::chrono::high_resolution_clock::now();

        for (int i{ 0 }; i < m_NumTests; ++i)
        {
            m_ComplexOutput = Brute::Opti_DiscreteFourierTransform(m_Source);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Buffer: " << m_BufferSize << " Cycles: " << m_NumTests << " DFT Opti: " << elapsed.count() << " seconds\n";

        CopyComplexToOutput();
        m_OutputFreq = Brute::GetPeakFreqThreshold(m_Output, m_PeakTreshold, m_SampleRate, m_BufferSize);
        m_OutputFreq = Brute::GetSimpleMeanFreq(m_OutputFreq, m_MeanTolerance);
    }
    ImGui::SameLine();
    if (ImGui::Button("Test DFT Literals"))
    {

        auto start = std::chrono::high_resolution_clock::now();
        for (int i{ 0 }; i < m_NumTests; ++i)
        {
            m_ComplexOutput = Brute::Opti_DiscreteFourierTransformLiterals(m_Source);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Buffer: " << m_BufferSize << " Cycles: " << m_NumTests << " Literal DFT Opti: " << elapsed.count() << " seconds\n";

        CopyComplexToOutput();
        m_OutputFreq = Brute::GetPeakFreqThreshold(m_Output, m_PeakTreshold, m_SampleRate, m_BufferSize);
        m_OutputFreq = Brute::GetSimpleMeanFreq(m_OutputFreq, m_MeanTolerance);
    }

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

void DFTSpeedTest::Reset()
{
}

void DFTSpeedTest::CopyComplexToOutput()
{
    m_Output.resize(m_ComplexOutput.size(), 0.);
    for (int i{ 0 }; i < m_ComplexOutput.size(); ++i)
    {
        m_Output[i] = std::abs(m_ComplexOutput[i]);
    }
}
