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

	float m_SampleRate;
	float m_TestFrequency;
	size_t m_BufferSize;
	std::vector<float> m_Source;
	std::vector<double> m_Output;
	std::vector<std::complex<double>> m_ComplexOutput;
};

