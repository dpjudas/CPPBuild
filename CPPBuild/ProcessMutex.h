#pragma once

class ProcessMutex
{
public:
	static std::unique_ptr<ProcessMutex> lock();

	virtual ~ProcessMutex() = default;
};
