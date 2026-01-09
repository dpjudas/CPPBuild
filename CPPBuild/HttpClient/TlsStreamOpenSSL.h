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
	void process();
	bool handleIOFailure(int result);

	void startReadSocket();
	void readSocketComplete(size_t bytesRead);

	void startWriteSocket();
	void writeSocketComplete(size_t bytesWritten);

	enum class TlsStreamState
	{
		Closed,
		Handshake,
		Open,
		Shutdown
	};

	TlsStreamState state = TlsStreamState::Closed;

	struct
	{
		uint8_t* data = nullptr;
		size_t size = 0;
		ReadCompleteCallback readComplete = {};
	} readState;

	struct
	{
		uint8_t* data = nullptr;
		size_t size = 0;
		WriteCompleteCallback writeComplete = {};
	} writeState;

	std::vector<char> readBuffer;
	size_t readBufferPos = 0;
	size_t readBufferAvailable = 0;
	bool readSocketOpen = true;

	std::vector<char> writeBuffer;
	size_t writeBufferPos = 0;
	size_t writeBufferSize = 0;

	AuthCompleteCallback authComplete = {};
	ShutdownCompleteCallback shutdownComplete = {};

	bool readSocketActive = false;
	bool writeSocketActive = false;

	ByteStream* socketStream = nullptr;
	SSL_CTX* ctx = nullptr;
	SSL* ssl = nullptr;
	BIO* internal_bio = nullptr;
	BIO* network_bio = nullptr;
};

