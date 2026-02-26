
#include "Precomp.h"
#include "FileTimeCache.h"
#include "IOData/File.h"
#include <mutex>
#include <unordered_map>
#include <stdexcept>

namespace
{
	std::unordered_map<std::string, std::optional<int64_t>> cache;
}

std::optional<int64_t> FileTimeCache::tryGetLastWriteTime(const std::string& filename)
{
	static std::mutex mutex;
	std::unique_lock lock(mutex);

	auto it = cache.find(filename);
	if (it != cache.end())
		return it->second;

	auto& item = cache[filename];
	item = File::tryGetLastWriteTime(filename);
	return item;
}

int64_t FileTimeCache::getLastWriteTime(const std::string& filename)
{
	auto result = tryGetLastWriteTime(filename);
	if (result.has_value())
		return result.value();
	throw std::runtime_error("Could not get last write time for: " + filename);
}
