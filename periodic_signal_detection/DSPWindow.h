#pragma once

class DSPWindow
{
public:
	DSPWindow() = default;
	virtual ~DSPWindow() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Reset() = 0;
};

