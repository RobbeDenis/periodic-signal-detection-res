#include "PrecisionTest.h"

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"
#include "SignalGenerators.h"
#include "BruteForceDSP.h"

#include <random>
#include <numeric>
#include <algorithm>

float PrecisionTest::GetRandomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

PrecisionTest::PrecisionTest()
	: DSPWindow()
    , m_bOutputChanged{ true }
    , m_SampleRate{ 4400.f }
    , m_BufferSize{ 44100 }
    , m_PeakTreshold{ 600. }
    , m_MeanTolerance{ 1. }
    , m_NumRndFreq{ 5 }
    , m_MinNoise{ -5.f }
    , m_MaxNoise{ 5.f }
    , m_MinFreq{ 0.1f }
    , m_MaxFreq{ 2100.f }
    , m_bApplyNoise{ false }
    , m_Source{ }
{
    BufferSizeChanged();
    m_SourceFreq.resize(m_NumRndFreq, 0.f);
}

PrecisionTest::~PrecisionTest()
{
}

void PrecisionTest::Init()
{
}

void PrecisionTest::Update()
{
    ImGui::Begin("Precision test");

    const float window_width = ImGui::GetContentRegionAvail().x;
    ImVec2 hSep{ 0.f, 2.f };
    ImVec2 vSep{ 30.f, 0.f };
    ImVec2 size{ window_width / 3.f - 5.f, 250.f };
    ImPlotFlags plotFlags{ ImPlotFlags_NoLegend };

    if (ImGui::Button("Calculate"))
    {
        SetRndFreq();
        m_DFTOutputComplex = Brute::Opti_DiscreteFourierTransform(m_Source);
        CopyComplexToOutput();
        m_DetectedPeaks = Brute::GetPeakFreqThreshold(m_DFTOutput, m_PeakTreshold, m_SampleRate, m_BufferSize);
        m_JointPeaksMean = Brute::GetSimpleMeanFreq(m_DetectedPeaks, m_MeanTolerance);
        m_DeltaPercentFreq = CalculatePercentageDelta(m_SourceFreq, m_JointPeaksMean);
        m_bOutputChanged = true;
    }

    ImGui::SameLine();
    ImGui::Text("<-- Random frequencies");

    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    if (ImPlot::BeginPlot("Source", size, plotFlags))
    {
        ImPlot::PlotLine("Source", m_Source.data(), static_cast<int>(m_Source.size()));

        ImPlot::EndPlot();
    }

    ImGui::SameLine();

    if (m_bOutputChanged)
        ImPlot::SetNextAxisToFit(ImAxis_X1);

    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    ImPlot::PushColormap(ImPlotColormap_Spectral);
    if (ImPlot::BeginPlot("DFT", size, plotFlags))
    {
        ImPlot::PlotLine("DFT", m_DFTOutput.data(), static_cast<int>(m_DFTOutput.size()));

        ImPlot::EndPlot();
    }

    ImGui::SameLine();

    ImPlot::SetNextAxisToFit(ImAxis_X1);
    ImPlot::SetNextAxisToFit(ImAxis_Y1);
    ImPlot::PushColormap(ImPlotColormap_Spectral);
    if (ImPlot::BeginPlot("Peaks", size))
    {
        ImPlot::PlotBars("Detected peaks", m_DetectedPeaks.data(), static_cast<int>(m_DetectedPeaks.size()));
        ImPlot::PlotBars("Joint peaks mean", m_JointPeaksMean.data(), static_cast<int>(m_JointPeaksMean.size()));

        ImPlot::EndPlot();
    }

    MainProperties(hSep, vSep);

    ImGui::BeginGroup();

    SourceStats(hSep, vSep);
    ImGui::SameLine();
    JointPeakStats(hSep, vSep);
    ImGui::SameLine();
    JointPeakWeightedStats(hSep, vSep);

    ImGui::SameLine();

    

    ImGui::EndGroup();

    if (m_bOutputChanged)
        m_bOutputChanged = false;

    ImGui::End();
}

void PrecisionTest::MainProperties(const ImVec2& hSep, const ImVec2& vSep)
{
    const float input_width{ 70.f };
    const float window_width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginGroup();

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

    /* Source */
    ImGui::BeginGroup();
    ImGui::Text("SOURCE");
    ImGui::Text("Samplerate: %.f", m_SampleRate);
    ImGui::Text("Buffer size: %d", m_BufferSize);
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::SameLine();

    /* Frequencies */
    ImGui::BeginGroup();
    ImGui::Text("FREQ");
    ImGui::Text("Min: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    ImGui::InputFloat("##MinFreq", &m_MinFreq, 0.0, 0.0, "%.2f");
    ImGui::Text("Max: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    ImGui::InputFloat("##MaxFreq", &m_MaxFreq, 0.0, 0.0, "%.2f");
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("NOISE");
    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &m_bApplyNoise);
    ImGui::Text("Min: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    ImGui::InputFloat("##MinNoise", &m_MinNoise, 0.0, 0.0, "%.2f");
    ImGui::Text("Max: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    ImGui::InputFloat("##MaxNoise", &m_MaxNoise, 0.0, 0.0, "%.2f");
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("PEAK DETECTION");
    ImGui::Text("Peak treshhold: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    ImGui::InputDouble("##PeakTreshold", &m_PeakTreshold, 0.0, 0.0, "%.2f");
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("JOINT PEAK MEAN");
    ImGui::Text("Mean tol:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    ImGui::InputDouble("##MeanTolerance", &m_MeanTolerance, 0.0, 0.0, "%.4f");
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

    ImGui::EndGroup();
}

void PrecisionTest::SourceStats(const ImVec2& hSep, const ImVec2& vSep)
{
    ImGui::BeginGroup();
    ImGui::Text("SOURCE");
    ImGui::Dummy(hSep);
    ImGui::Text("Frequencies (Hz)");

    ImGui::BeginGroup();
    for (float freq : m_SourceFreq)
    {
        ImGui::Text("%.5f", freq);
    }
    ImGui::EndGroup();
    ImGui::Dummy(hSep);
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
}

void PrecisionTest::JointPeakStats(const ImVec2& hSep, const ImVec2& vSep)
{
    ImGui::BeginGroup();

    ImGui::Text("JOINT PEAK MEAN");
    ImGui::Dummy(hSep);

    ImGui::BeginGroup();
    ImGui::Text("Frequencies (Hz)");
    for (double freq : m_JointPeaksMean)
    {
        ImGui::Text("%.5f", freq);
    }
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Delta (%%)");
    for (double delta : m_DeltaPercentFreq)
    {
        SetDeltaColorFreq(delta);
        ImGui::Text("%.5f", delta);
        ImGui::PopStyleColor();
    }
    ImGui::EndGroup();

    ImGui::Dummy(hSep);

    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
}

void PrecisionTest::JointPeakWeightedStats(const ImVec2& hSep, const ImVec2& vSep)
{
    ImGui::BeginGroup();

    ImGui::Text("PEAK MEAN LOG");
    ImGui::Dummy(hSep);

    ImGui::Dummy(hSep);

    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(vSep);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
}

void PrecisionTest::CopyComplexToOutput()
{
    m_DFTOutput.resize(m_DFTOutputComplex.size(), 0.);
    for (int i{ 0 }; i < m_DFTOutputComplex.size(); ++i)
    {
        m_DFTOutput[i] = std::abs(m_DFTOutputComplex[i]);
    }
}

void PrecisionTest::SetRndFreq()
{
    std::fill(begin(m_Source), end(m_Source), 0.f);
    for (int i{ 0 }; i < m_NumRndFreq; ++i)
    {
        const float freq{ GetRandomFloat(m_MinFreq, m_MaxFreq) };
        m_SourceFreq[i] = freq;
        Generate::Sine(m_Source, 1.f, freq, m_SampleRate);
    }

    if (m_bApplyNoise)
    {
        Generate::RndNoise(m_Source, m_MinNoise, m_MaxNoise);
    }

    std::sort(begin(m_SourceFreq), end(m_SourceFreq));
}

std::vector<double> PrecisionTest::CalculatePercentageDelta(const std::vector<float>& input, const std::vector<double>& output)
{
    std::vector<double> delta;
    size_t j = 0;

    for (size_t i = 0; i < input.size(); ++i) 
    {
        double in_val = static_cast<double>(input[i]);

        while (j + 1 < output.size() && std::abs(output[j + 1] - in_val) < std::abs(output[j] - in_val)) 
        {
            ++j;
        }

        double out_val = output[j];

        if (in_val != 0.) 
        {
            double percent_diff = ((out_val - in_val) / in_val) * 100.;
            delta.push_back(percent_diff);
        }
        else 
        {
            delta.push_back(0.);
        }
    }

    return delta;
}

void PrecisionTest::SetDeltaColorFreq(double delta)
{
    const double absDelta{ std::abs(delta) };

    if (absDelta > 0.25)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    else if (absDelta > 0.05)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 125, 125, 255));
    else if (absDelta > 0.01)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    else if (absDelta > 0.001)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 255, 125, 255));
    else
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));

}

void PrecisionTest::BufferSizeChanged()
{
    m_Source.resize(m_BufferSize, 0.f);
    m_DFTOutput.reserve(m_BufferSize / 2);
    m_DFTOutputComplex.reserve(m_BufferSize / 2);
}

void PrecisionTest::Reset()
{
}