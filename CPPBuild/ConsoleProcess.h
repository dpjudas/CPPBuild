#pragma once

#include <string>
#include <functional>

class ConsoleProcess
{
public:
	static int runCommand(const std::string& commandline, const std::function<void(const std::string& outputLine)> outputCallback);
};
