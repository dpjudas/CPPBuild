#pragma once

#include "TlsStream.h"
#include "SocketHeaders.h"

#define SECURITY_WIN32
#include <security.h>
#include <schannel.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <memory>
#include <algorithm>
#include <string>

class TlsStreamWin32 : public TlsStream
{
public:
	TlsStreamWin32(ByteStream* socketStream);
	~TlsStreamWin32();

	typedef std::function<void()> AuthCompleteCallback;
	typedef std::function<void()> ShutdownCompleteCallback;

	void authenticateAsServer(AuthCompleteCallback authComplete, const void* pfxData = nullptr, size_t pfxSize = 0, const char* pfxPassword = nullptr) override;
	void authenticateAsClient(AuthCompleteCallback authComplete, const std::string& targetName) override;

	void write(const void* data, size_t size, WriteCompleteCallback writeComplete) override;
	void read(void* data, size_t size, ReadCompleteCallback readComplete) override;

	void shutdown(ShutdownCompleteCallback shutdownComplete);

private:
	void process();
	void startReadSocket();
	void readSocketComplete(size_t bytesRead);
	void startWriteSocket();
	void writeSocketComplete(size_t bytesWritten);

	typedef PCCERT_CONTEXT CertHandle;

	void loadPfxCredentials(const void* data, size_t size, const wchar_t* password);
	void createCredentials();
	void handshakeHello();
	void handshakeAccept();
	void handshakeLoop();
	void readDecrypt();
	size_t writeEncrypt(const void* data, size_t size);
	void shutdownTLS();
	void freeCertificate(CertHandle cert);

	std::wstring to_utf16(const std::string& str);

	ByteStream* socketStream = nullptr;

	WriteCompleteCallback writeComplete = {};
	size_t encryptedBytesWritten = 0;

	struct
	{
		uint8_t* data = nullptr;
		size_t size = 0;
		ReadCompleteCallback readComplete = {};
	} readState;

	AuthCompleteCallback authComplete = {};
	ShutdownCompleteCallback shutdownComplete = {};

	enum class TlsStreamState
	{
		Closed,
		HandshakeAccept,
		HandshakeLoop,
		Open,
		Shutdown
	};

	TlsStreamState state = TlsStreamState::Closed;

	CredHandle credHandle = {};
	CtxtHandle contextHandle = {};

	const DWORD sspiClientFlags = ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_CONFIDENTIALITY | ISC_RET_EXTENDED_ERROR | ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_STREAM;
	const DWORD sspiServerFlags = ASC_REQ_ALLOCATE_MEMORY | ASC_REQ_CONFIDENTIALITY | ASC_RET_EXTENDED_ERROR | ASC_REQ_SEQUENCE_DETECT | ASC_REQ_REPLAY_DETECT | ASC_REQ_STREAM;

	std::vector<char> readBuffer;
	size_t readBufferAvailable = 0;
	bool readSocketOpen = true;

	SecPkgContext_StreamSizes streamSizes = {};

	size_t decryptedDataPos = 0;
	size_t decryptedDataEnd = 0;
	size_t decryptedDataNextMessagePos = 0;

	std::vector<char> encryptBuffer;
	int encryptBufferPos = 0;
	int encryptBufferEnd = 0;

	std::vector<SecBuffer> sendBuffers;
	size_t sendBufferPos = 0;

	bool readSocketActive = false;
	bool writeSocketActive = false;

	bool client = true;
	std::wstring targetName;
};
