#pragma once

#include "TlsStream.h"
#include "SocketHeaders.h"

class TlsStreamOpenSSL : public TlsStream
{
public:
	TlsStreamOpenSSL(ByteStream* socketStream);
	~TlsStreamOpenSSL();

	void authenticateAsServer(AuthCompleteCallback authComplete, const void* pfxData = nullptr, size_t pfxSize = 0, const char* pfxPassword = nullptr) override;
	void authenticateAsClient(AuthCompleteCallback authComplete, const std::string& targetName) override;

	void write(const void* data, size_t size, WriteCompleteCallback writeComplete) override;
	void read(void* data, size_t size, ReadCompleteCallback readComplete) override;

	void shutdown(ShutdownCompleteCallback shutdownComplete) override;

private:
	ByteStream* socketStream = nullptr;
};

