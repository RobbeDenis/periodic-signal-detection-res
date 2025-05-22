#pragma once
#include "DSPWindow.h"
#include <vector>

class FourierTransormTest final : public DSPWindow
{
public:
	FourierTransormTest();
	~FourierTransormTest();

	void Init() override;
	void Update() override;
	void Reset() override;

private:
	float m_SampleRate;
	size_t m_BufferSize;
	std::vector<float> m_Source;
};

