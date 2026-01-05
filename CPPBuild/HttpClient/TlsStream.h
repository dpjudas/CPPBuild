#pragma once

#include "ByteStream.h"

class TlsStream : public ByteStream
{
public:
	static std::unique_ptr<TlsStream> create(ByteStream* socketStream);

	typedef std::function<void()> AuthCompleteCallback;
	typedef std::function<void()> ShutdownCompleteCallback;

	virtual void authenticateAsServer(AuthCompleteCallback authComplete, const void* pfxData = nullptr, size_t pfxSize = 0, const char* pfxPassword = nullptr) = 0;
	virtual void authenticateAsClient(AuthCompleteCallback authComplete, const std::string& targetName) = 0;
	virtual void shutdown(ShutdownCompleteCallback shutdownComplete) = 0;
};
