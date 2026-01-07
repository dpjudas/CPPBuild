
#include "Precomp.h"
#include "HttpConnectionImpl.h"
#include "TlsStream.h"
#include "SocketStream.h"
#include <stdexcept>
#include <cstring>

HttpConnectionImpl::HttpConnectionImpl()
{
	TcpSocket::init();
}

HttpConnectionImpl::~HttpConnectionImpl()
{
}

void HttpConnectionImpl::connect(const HttpUri& url)
{
	if (url.scheme == "https")
	{
		socketstream = std::make_unique<SocketStream>(url.host, url.port != 0 ? url.port : 443);

		tlsstream = TlsStream::create(socketstream.get());
		bool done = false;
		tlsstream->authenticateAsClient([&]() { done = true; }, url.host);
		while (!done)
			socketstream->process();

		datastream = tlsstream.get();
	}
	else
	{
		socketstream = std::make_unique<SocketStream>(url.host, url.port != 0 ? url.port : 80);
		datastream = socketstream.get();
	}
}

void HttpConnectionImpl::writeAll(const void* data, size_t size)
{
	if (size == 0) return;

	size_t writepos = 0;
	std::function<void(size_t bytesWritten)> writeComplete;
	writeComplete = [&](size_t bytesWritten)
		{
			writepos += bytesWritten;
			if (writepos != size)
			{
				datastream->write((uint8_t*)data + writepos, size - writepos, writeComplete);
			}
		};
	writeComplete(0);

	while (writepos != size)
		socketstream->process();
}

void HttpConnectionImpl::readAll(void* data, size_t size)
{
	size_t pos = 0;
	while (pos < size)
	{
		pos += read((uint8_t*)data + pos, size - pos);
	}
}

std::string HttpConnectionImpl::readLine()
{
	if (!linebuffer) linebuffer.reset(new uint8_t[linebuffersize]);

	while (true)
	{
		uint8_t* data = linebuffer.get();
		for (size_t i = linebufferpos; i < linebufferend; i++)
		{
			if (data[i] == '\n')
			{
				std::string line;
				if (i > 0 && data[i - 1] == '\r')
					line = std::string(data + linebufferpos, data + i - 1);
				else
					line = std::string(data + linebufferpos, data + i);
				linebufferpos = i + 1;
				return line;
			}
		}
		memmove(data, data + linebufferpos, linebufferend - linebufferpos);
		linebufferend -= linebufferpos;
		linebufferpos = 0;

		if (linebufferend == linebuffersize)
			throw std::runtime_error("HTTP line longer than " + std::to_string(linebuffersize) + " bytes");
		linebufferend += read(data + linebufferend, linebuffersize - linebufferend);
	}
}

size_t HttpConnectionImpl::read(void* data, size_t size)
{
	if (size == 0) return 0;

	if (linebuffer && linebufferpos != linebufferend)
	{
		size = std::min(linebufferend - linebufferpos, size);
		memcpy(data, linebuffer.get() + linebufferpos, size);
		linebufferpos += size;
		return size;
	}
	else
	{
		bool done = false;
		size_t result = 0;
		datastream->read(data, size, [&](size_t bytesRead) {
			result = bytesRead;
			done = true;
			});

		while (!done)
			socketstream->process();

		return result;
	}
}
