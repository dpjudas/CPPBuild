#pragma once

#include <array>
#include <memory>
#include <functional>

class ByteStream
{
public:
	typedef std::function<void(size_t bytesWritten)> WriteCompleteCallback;
	typedef std::function<void(size_t bytesRead)> ReadCompleteCallback;
	virtual ~ByteStream() = default;
	virtual void write(const void* data, size_t size, WriteCompleteCallback writeComplete) = 0;
	virtual void read(void* data, size_t size, ReadCompleteCallback readComplete) = 0;
};
