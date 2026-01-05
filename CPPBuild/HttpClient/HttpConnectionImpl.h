#pragma once

#include "HttpConnection.h"
#include "HttpClient.h"

class ByteStream;
class SocketStream;
class TlsStream;

class HttpConnectionImpl : public HttpConnection
{
public:
	HttpConnectionImpl();
	~HttpConnectionImpl();

	void connect(const HttpUri& url) override;
	void writeAll(const void* data, size_t size) override;
	void readAll(void* data, size_t size) override;
	std::string readLine() override;

private:
	size_t read(void* data, size_t size);

	std::unique_ptr<SocketStream> socketstream;
	std::unique_ptr<TlsStream> tlsstream;
	ByteStream* datastream = nullptr;

	static const size_t linebuffersize = 4 * 1024;
	std::unique_ptr<uint8_t[]> linebuffer;
	size_t linebufferpos = 0, linebufferend = 0;
};
