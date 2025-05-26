#pragma once
#include "DSPWindow.h"

#include <vector>
#include <complex>

struct ImVec2;

class PrecisionTest final : public DSPWindow
{
public:
	PrecisionTest();
	~PrecisionTest();

	void Init() override;
	void Update() override;
	void Reset() override;

private:
	void MainProperties(const ImVec2& hSep, const ImVec2& vSep);
	void SourceStats(const ImVec2& hSep, const ImVec2& vSep);
	void JointPeakStats(const ImVec2& hSep, const ImVec2& vSep);
	void JointPeakWeightedStats(const ImVec2& hSep, const ImVec2& vSep);

	void CopyComplexToOutput();
	void SetRndFreq();
	void BufferSizeChanged();

	void SetDeltaColorFreq(double delta);

	float GetRandomFloat(float min, float max);
	std::vector<double> CalculatePercentageDelta(const std::vector<float>& input, const std::vector<double>& output);

	bool m_bOutputChanged;
	bool m_bApplyNoise;
	float m_SampleRate;
	float m_MinNoise;
	float m_MaxNoise;
	float m_MinFreq;
	float m_MaxFreq;
	double m_PeakTreshold;
	double m_MeanTolerance;
	int m_BufferSize;
	int m_NumRndFreq;

	std::vector<float> m_Source;
	std::vector<float> m_SourceFreq;
	std::vector<double> m_DeltaPercentFreq;
	std::vector<double> m_DFTOutput;
	std::vector<double> m_DetectedPeaks;
	std::vector<double> m_JointPeaksMean;
	std::vector<std::complex<double>> m_DFTOutputComplex;
};

