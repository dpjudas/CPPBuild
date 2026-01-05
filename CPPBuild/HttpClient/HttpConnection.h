#pragma once

#include <memory>
#include <string>

class HttpUri;

class HttpConnection
{
public:
	static std::unique_ptr<HttpConnection> create();

	virtual ~HttpConnection() = default;
	virtual void connect(const HttpUri& url) = 0;
	virtual void writeAll(const void* data, size_t size) = 0;
	virtual void readAll(void* data, size_t size) = 0;
	virtual std::string readLine() = 0;
};
