#pragma once

#include <optional>

class FileTimeCache
{
public:
	static std::optional<int64_t> tryGetLastWriteTime(const std::string& filename);
	static int64_t getLastWriteTime(const std::string& filename);
};
