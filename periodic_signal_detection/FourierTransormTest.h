#pragma once
#include "DSPWindow.h"
#include <vector>
#include <complex>

class FourierTransormTest final : public DSPWindow
{
public:
	FourierTransormTest();
	~FourierTransormTest();

	void Init() override;
	void Update() override;
	void Reset() override;

private:
	void CopyComplexToOutput();
	void InfoPeakAndDip();
	void InfoPeakFrequencies();

	float m_SampleRate;
	float m_TestFrequency;
	double m_PeakTreshold;
	double m_MeanTolerance;
	size_t m_BufferSize;
	std::vector<float> m_Source;
	std::vector<double> m_Output;
	std::vector<double> m_OutputFreq;
	std::vector<std::complex<double>> m_ComplexOutput;
};

