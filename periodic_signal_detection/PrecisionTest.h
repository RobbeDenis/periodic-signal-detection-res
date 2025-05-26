#pragma once
#include "DSPWindow.h"

#include <vector>
#include <complex>

class PrecisionTest final : public DSPWindow
{
public:
	PrecisionTest();
	~PrecisionTest();

	void Init() override;
	void Update() override;
	void Reset() override;

private:
	void MainProperties();

	void CopyComplexToOutput();
	void SetRndFreq();
	void SetDeltaColorFreq(double delta);
	void BufferSizeChanged();

	float GetRandomFloat(float min, float max);
	std::vector<double> CalculatePercentageDelta(const std::vector<float>& input, const std::vector<double>& output);

	bool m_bOutputChanged;
	bool m_bApplyNoise;
	float m_SampleRate;
	float m_MinNoise;
	float m_MaxNoise;
	double m_SourcePeriodMs;
	double m_PeakTreshold;
	double m_MeanTolerance;
	int m_BufferSize;
	size_t m_NumRndFreq;

	std::vector<float> m_Source;
	std::vector<float> m_SourceFreq;
	std::vector<double> m_DeltaPercent;
	std::vector<double> m_DFTOutput;
	std::vector<double> m_DetectedPeaks;
	std::vector<double> m_Mean;
	std::vector<std::complex<double>> m_DFTOutputComplex;
};

