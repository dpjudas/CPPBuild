#pragma once

#include "TlsStream.h"
#include "SocketHeaders.h"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

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
	bool processIO(int result);
	void startSocketRead();
	void startSocketWrite();

	enum class State
	{
		closed,
		connecting,
		open,
		shuttingDown
	};
	State state = State::closed;

	ByteStream* socketStream = nullptr;
	SSL_CTX* ctx = nullptr;
	SSL* ssl = nullptr;
	BIO* internal_bio = nullptr;
	BIO* network_bio = nullptr;
};

