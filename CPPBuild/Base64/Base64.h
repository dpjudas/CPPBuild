
#pragma once

class DataBuffer;

class Base64
{
public:
	static std::string encode(const void* data, size_t size);
	static std::string encode(const std::string& data);
	static std::string encode(std::shared_ptr<DataBuffer> data);

	static std::string decodeText(const void* data, size_t size);
	static std::string decodeText(const std::string& data);
	static std::shared_ptr<DataBuffer> decode(const void* data, size_t size);
	static std::shared_ptr<DataBuffer> decode(const std::string& data);
};
