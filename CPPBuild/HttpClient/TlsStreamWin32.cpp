
#include "Precomp.h"
#include "TlsStreamWin32.h"
#include <stdexcept>
#include <algorithm>
#include <string>

#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Ncrypt.lib")

TlsStreamWin32::TlsStreamWin32(ByteStream* socketStream) : socketStream(socketStream)
{
}

TlsStreamWin32::~TlsStreamWin32()
{
	if (state != TlsStreamState::Closed)
	{
		for (SecBuffer buffer : sendBuffers)
		{
			FreeContextBuffer(buffer.pvBuffer);
		}
		DeleteSecurityContext(&contextHandle);
		FreeCredentialsHandle(&credHandle);
	}
}

void TlsStreamWin32::authenticateAsServer(AuthCompleteCallback authCompleteCallback, const void* pfxData, size_t pfxSize, const char* pfxPassword)
{
	if (state != TlsStreamState::Closed)
		throw std::runtime_error("TLS session already open");

	client = false;
	if (pfxData)
		loadPfxCredentials(pfxData, pfxSize, pfxPassword ? to_utf16(pfxPassword).c_str() : nullptr);
	else
		createCredentials();
	state = TlsStreamState::HandshakeAccept;
	authComplete = authCompleteCallback;
	process();
}

void TlsStreamWin32::authenticateAsClient(AuthCompleteCallback authCompleteCallback, const std::string& servertargetName)
{
	if (state != TlsStreamState::Closed)
		throw std::runtime_error("TLS session already open");

	client = true;
	targetName = to_utf16(servertargetName);
	createCredentials();
	handshakeHello();
	state = TlsStreamState::HandshakeLoop;
	authComplete = authCompleteCallback;
	process();
}

void TlsStreamWin32::shutdown(ShutdownCompleteCallback shutdownCompleteCallback)
{
	if (state == TlsStreamState::Closed)
		return;

	if (state == TlsStreamState::Open)
		shutdownTLS();

	state = TlsStreamState::Shutdown;
	shutdownComplete = shutdownCompleteCallback;

	process();
}

void TlsStreamWin32::startReadSocket()
{
	if (readSocketActive)
		return;

	if (readBuffer.size() < 0x10000)
		readBuffer.resize(0x10000);

	if (readSocketOpen && readBufferAvailable < readBuffer.size())
	{
		readSocketActive = true;
		socketStream->read(readBuffer.data() + readBufferAvailable, readBuffer.size() - readBufferAvailable, [this](size_t bytesRead) { readSocketComplete(bytesRead); });
	}
}

void TlsStreamWin32::readSocketComplete(size_t bytesRead)
{
	readSocketOpen = (bytesRead != 0);
	readBufferAvailable += bytesRead;
	readSocketActive = false;
	process();
}

void TlsStreamWin32::startWriteSocket()
{
	if (writeSocketActive)
		return;

	if (!sendBuffers.empty())
	{
		const auto& buffer = sendBuffers.front();
		writeSocketActive = true;
		socketStream->write(buffer.pvBuffer, buffer.cbBuffer - sendBufferPos, [this](size_t bytesWritten) { writeSocketComplete(bytesWritten); });
	}
	else if (encryptBufferPos < encryptBufferEnd)
	{
		writeSocketActive = true;
		socketStream->write(encryptBuffer.data() + encryptBufferPos, (size_t)encryptBufferEnd - encryptBufferPos, [this](size_t bytesWritten) { writeSocketComplete(bytesWritten); });
	}
}

void TlsStreamWin32::writeSocketComplete(size_t bytesWritten)
{
	if (!sendBuffers.empty())
	{
		const auto& buffer = sendBuffers.front();
		sendBufferPos += bytesWritten;
		if (sendBufferPos == buffer.cbBuffer)
		{
			FreeContextBuffer(buffer.pvBuffer);
			sendBuffers.erase(sendBuffers.begin());
			sendBufferPos = 0;
		}
	}
	else
	{
		encryptBufferPos += (int)bytesWritten;
		if (encryptBufferPos == encryptBufferEnd)
		{
			encryptBufferPos = 0;
			encryptBufferEnd = 0;
			writeComplete(encryptedBytesWritten);
		}
	}
	writeSocketActive = false;
	process();
}

void TlsStreamWin32::process()
{
	switch (state)
	{
	default:
	case TlsStreamState::Closed:
		break;
	case TlsStreamState::Open:
		readDecrypt();
		break;
	case TlsStreamState::HandshakeAccept:
		handshakeAccept();
		break;
	case TlsStreamState::HandshakeLoop:
		handshakeLoop();
		break;
	case TlsStreamState::Shutdown:
		if (sendBuffers.empty())
		{
			DeleteSecurityContext(&contextHandle);
			FreeCredentialsHandle(&credHandle);
			contextHandle = {};
			credHandle = {};
			state = TlsStreamState::Closed;
			shutdownComplete();
		}
		break;
	}

	startWriteSocket();
}

void TlsStreamWin32::write(const void* data, size_t size, WriteCompleteCallback writeCompleteCallback)
{
	if (state != TlsStreamState::Open)
		throw std::runtime_error("TLS session not open");

	writeComplete = writeCompleteCallback;
	encryptedBytesWritten = writeEncrypt(static_cast<const uint8_t*>(data), size);

	process();
}

void TlsStreamWin32::read(void* data, size_t size, ReadCompleteCallback readComplete)
{
	readState.data = (uint8_t*)data;
	readState.size = size;
	readState.readComplete = readComplete;
	process();
}

void TlsStreamWin32::handshakeHello()
{
	SecBufferDesc OutBuffer;
	SecBuffer OutBuffers[1];

	OutBuffers[0].pvBuffer = nullptr;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = 0;
	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	DWORD sspiOutFlags;
	TimeStamp tsExpiry;
	SECURITY_STATUS result;

	// Initiate a ClientHello message and generate a token.
	result = InitializeSecurityContext(&credHandle, nullptr, targetName.empty() ? nullptr : (SEC_WCHAR*)targetName.c_str(), sspiClientFlags, 0, SECURITY_NATIVE_DREP, nullptr, 0, &contextHandle, &OutBuffer, &sspiOutFlags, &tsExpiry);
	if (result != SEC_I_CONTINUE_NEEDED)
		throw std::runtime_error("InitializeSecurityContext failed");

	if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer)
	{
		sendBuffers.push_back(OutBuffers[0]);
	}
}

void TlsStreamWin32::handshakeAccept()
{
	if (readBufferAvailable == 0)
	{
		startReadSocket();
		return;
	}

	SecBufferDesc OutBuffer, InBuffer;
	SecBuffer OutBuffers[1], InBuffers[2];

	OutBuffers[0].pvBuffer = nullptr;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = 0;
	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	InBuffers[0].pvBuffer = readBuffer.data();
	InBuffers[0].cbBuffer = (DWORD)readBufferAvailable;
	InBuffers[0].BufferType = SECBUFFER_TOKEN;
	InBuffers[1].pvBuffer = nullptr;
	InBuffers[1].cbBuffer = 0;
	InBuffers[1].BufferType = SECBUFFER_EMPTY;
	InBuffer.cBuffers = 2;
	InBuffer.pBuffers = InBuffers;
	InBuffer.ulVersion = SECBUFFER_VERSION;

	DWORD sspiOutFlags;
	TimeStamp tsExpiry;
	SECURITY_STATUS result;

	result = AcceptSecurityContext(&credHandle, nullptr, &InBuffer, sspiServerFlags, 0, &contextHandle, &OutBuffer, &sspiOutFlags, &tsExpiry);
	if (result == SEC_E_INCOMPLETE_MESSAGE)
	{
		startReadSocket();
		return;
	}

	if (result == SEC_E_ALGORITHM_MISMATCH)
		throw std::runtime_error("The client and server cannot communicate, because they do not possess a common algorithm");
	if (result != SEC_I_CONTINUE_NEEDED)
		throw std::runtime_error("AcceptSecurityContext failed");

	if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer)
	{
		sendBuffers.push_back(OutBuffers[0]);
	}

	// Move extra input data to front of input buffer
	if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
	{
		MoveMemory(readBuffer.data(), readBuffer.data() + (readBufferAvailable - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);
		readBufferAvailable = InBuffers[1].cbBuffer;
	}
	else
	{
		readBufferAvailable = 0;
	}

	state = TlsStreamState::HandshakeLoop;
	handshakeLoop();
}

void TlsStreamWin32::handshakeLoop()
{
	// Keep reading and writing data until handshake has completed or we need more data
	while (true)
	{
		if (readBufferAvailable == 0)
		{
			startReadSocket();
			return;
		}

		SecBufferDesc OutBuffer, InBuffer;
		SecBuffer InBuffers[2], OutBuffers[1];

		// Set up the input buffers. Buffer 0 is used to pass in data received from the server. Schannel will consume some or all
		// of this. Leftover data (if any) will be placed in buffer 1 and given a buffer type of SECBUFFER_EXTRA.
		InBuffers[0].pvBuffer = readBuffer.data();
		InBuffers[0].cbBuffer = (DWORD)readBufferAvailable;
		InBuffers[0].BufferType = SECBUFFER_TOKEN;
		InBuffers[1].pvBuffer = nullptr;
		InBuffers[1].cbBuffer = 0;
		InBuffers[1].BufferType = SECBUFFER_EMPTY;
		InBuffer.cBuffers = 2;
		InBuffer.pBuffers = InBuffers;
		InBuffer.ulVersion = SECBUFFER_VERSION;

		// Set up the output buffers
		OutBuffers[0].pvBuffer = nullptr;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;
		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		DWORD sspiOutFlags;
		TimeStamp tsExpiry;
		SECURITY_STATUS result;
		if (client)
		{
			result = InitializeSecurityContext(&credHandle, &contextHandle, targetName.empty() ? nullptr : (SEC_WCHAR*)targetName.c_str(), sspiClientFlags, 0, SECURITY_NATIVE_DREP, &InBuffer, 0, nullptr, &OutBuffer, &sspiOutFlags, &tsExpiry);
		}
		else
		{
			result = AcceptSecurityContext(&credHandle, &contextHandle, &InBuffer, sspiServerFlags, 0, nullptr, &OutBuffer, &sspiOutFlags, &tsExpiry);
		}

		// Write output, if available
		if (result == SEC_E_OK || result == SEC_I_CONTINUE_NEEDED || (FAILED(result) && (sspiOutFlags & ISC_RET_EXTENDED_ERROR)))
		{
			if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer)
			{
				sendBuffers.push_back(OutBuffers[0]);
			}
		}

		if (result == SEC_E_INCOMPLETE_MESSAGE)
		{
			startReadSocket();
			return;
		}
		else
		{
			if (FAILED(result))
			{
				if (client)
					throw std::runtime_error("InitializeSecurityContext failed");
				else
					throw std::runtime_error("AcceptSecurityContext failed");
			}

			// Move extra input data to front of input buffer
			if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
			{
				MoveMemory(readBuffer.data(), readBuffer.data() + (readBufferAvailable - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);
				readBufferAvailable = InBuffers[1].cbBuffer;
			}
			else
			{
				readBufferAvailable = 0;
			}

			if (result == SEC_I_INCOMPLETE_CREDENTIALS)
			{
				throw std::runtime_error("Server wants a client certificate");
			}
			else if (result == SEC_E_OK || result != SEC_I_CONTINUE_NEEDED)
			{
				break;
			}
		}
	}

	SECURITY_STATUS result = QueryContextAttributes(&contextHandle, SECPKG_ATTR_STREAM_SIZES, &streamSizes);
	if (FAILED(result))
		throw std::runtime_error("QueryContextAttributes(SECPKG_ATTR_STREAM_SIZES) failed");

	readBuffer.resize((size_t)streamSizes.cbHeader + streamSizes.cbMaximumMessage + streamSizes.cbTrailer);
	encryptBuffer.resize((size_t)streamSizes.cbHeader + streamSizes.cbMaximumMessage + streamSizes.cbTrailer);

	state = TlsStreamState::Open;
	if (authComplete)
	{
		auto callback = std::move(authComplete);
		authComplete = {};
		callback();
	}
}

void TlsStreamWin32::readDecrypt()
{
	if (readState.size == 0)
		return;

	if (decryptedDataPos == decryptedDataEnd && readSocketOpen)
	{
		// The encrypted message is decrypted in place, overwriting the original contents of its buffer.

		SecBuffer Buffers[4];
		SecBufferDesc message;

		Buffers[0].pvBuffer = readBuffer.data() + decryptedDataNextMessagePos;
		Buffers[0].cbBuffer = (DWORD)(readBufferAvailable - decryptedDataNextMessagePos);
		Buffers[0].BufferType = SECBUFFER_DATA;
		Buffers[1].BufferType = SECBUFFER_EMPTY;
		Buffers[2].BufferType = SECBUFFER_EMPTY;
		Buffers[3].BufferType = SECBUFFER_EMPTY;

		message.ulVersion = SECBUFFER_VERSION;
		message.cBuffers = 4;
		message.pBuffers = Buffers;

		SECURITY_STATUS result = DecryptMessage(&contextHandle, &message, 0, nullptr);
		if (result == SEC_E_INCOMPLETE_MESSAGE)
		{
			if (decryptedDataNextMessagePos)
			{
				size_t extraDataSize = readBufferAvailable - decryptedDataNextMessagePos;
				MoveMemory(readBuffer.data(), readBuffer.data() + decryptedDataNextMessagePos, extraDataSize);
				readBufferAvailable = extraDataSize;
				decryptedDataNextMessagePos = 0;
			}
			startReadSocket();
			return;
		}
		else if (result == SEC_I_CONTEXT_EXPIRED) // Server signalled end of session
		{
			readSocketOpen = false;
			readState.size = 0;
			readState.readComplete(0);
			return;
		}
		else if (result == SEC_E_DECRYPT_FAILURE)
		{
			throw std::runtime_error("DecryptMessage returned decrypt failure");
		}
		else if (result != SEC_E_OK && result != SEC_I_RENEGOTIATE)
		{
			throw std::runtime_error("Unexpected return value from DecryptMessage");
		}

		SecBuffer* dataBuffer = nullptr;
		SecBuffer* extraBuffer = nullptr;
		for (int i = 1; i < 4; i++)
		{
			if (dataBuffer == nullptr && Buffers[i].BufferType == SECBUFFER_DATA) dataBuffer = &Buffers[i];
			if (extraBuffer == nullptr && Buffers[i].BufferType == SECBUFFER_EXTRA) extraBuffer = &Buffers[i];
		}

		if (dataBuffer)
		{
			decryptedDataPos = (ptrdiff_t)((char*)dataBuffer->pvBuffer - readBuffer.data());
			decryptedDataEnd = decryptedDataPos + dataBuffer->cbBuffer;
		}

		if (extraBuffer)
		{
			decryptedDataNextMessagePos = (ptrdiff_t)((char*)extraBuffer->pvBuffer - readBuffer.data());
		}
		else
		{
			decryptedDataNextMessagePos = readBufferAvailable;
		}

		if (result == SEC_I_RENEGOTIATE)
			state = TlsStreamState::HandshakeLoop;
	}

	size_t size = std::min(readState.size, decryptedDataEnd - decryptedDataPos);
	memcpy(readState.data, readBuffer.data() + decryptedDataPos, size);
	decryptedDataPos += size;
	readState.size = 0;
	readState.readComplete(size);
}

size_t TlsStreamWin32::writeEncrypt(const void* data, size_t len)
{
	// The encrypted message is encrypted in place, overwriting the original contents of its buffer.

	size_t available = encryptBuffer.size() - encryptBufferEnd - streamSizes.cbHeader - streamSizes.cbTrailer;
	available = std::min(available, (size_t)streamSizes.cbMaximumMessage);
	len = std::min(len, available);

	memcpy(encryptBuffer.data() + encryptBufferEnd + streamSizes.cbHeader, data, len);

	SecBuffer Buffers[4];
	Buffers[0].pvBuffer = encryptBuffer.data();
	Buffers[0].cbBuffer = streamSizes.cbHeader;
	Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;
	Buffers[1].pvBuffer = encryptBuffer.data() + streamSizes.cbHeader;
	Buffers[1].cbBuffer = (unsigned long)len;
	Buffers[1].BufferType = SECBUFFER_DATA;
	Buffers[2].pvBuffer = encryptBuffer.data() + streamSizes.cbHeader + len;
	Buffers[2].cbBuffer = streamSizes.cbTrailer;
	Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
	Buffers[3].pvBuffer = SECBUFFER_EMPTY;
	Buffers[3].cbBuffer = SECBUFFER_EMPTY;
	Buffers[3].BufferType = SECBUFFER_EMPTY;

	SecBufferDesc Message;
	Message.ulVersion = SECBUFFER_VERSION;
	Message.cBuffers = 4;
	Message.pBuffers = Buffers;
	SECURITY_STATUS result = EncryptMessage(&contextHandle, 0, &Message, 0);
	if (FAILED(result))
		throw std::runtime_error("EncryptMessage failed");

	encryptBufferEnd += (int)(streamSizes.cbHeader + len + streamSizes.cbTrailer);

	return len;
}

void TlsStreamWin32::shutdownTLS()
{
	SecBufferDesc OutBuffer;
	SecBuffer OutBuffers[1];

	DWORD type = SCHANNEL_SHUTDOWN;

	OutBuffers[0].pvBuffer = &type;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = sizeof(DWORD);
	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	SECURITY_STATUS status = ApplyControlToken(&contextHandle, &OutBuffer);
	if (FAILED(status))
		throw std::runtime_error("ApplyControlToken failed");

	OutBuffers[0].pvBuffer = nullptr;
	OutBuffers[0].BufferType = SECBUFFER_TOKEN;
	OutBuffers[0].cbBuffer = 0;
	OutBuffer.cBuffers = 1;
	OutBuffer.pBuffers = OutBuffers;
	OutBuffer.ulVersion = SECBUFFER_VERSION;

	TimeStamp tsExpiry;
	DWORD sspiOutFlags = 0;
	if (client)
	{
		status = InitializeSecurityContext(&credHandle, &contextHandle, targetName.empty() ? nullptr : (SEC_WCHAR*)targetName.c_str(), sspiClientFlags, 0, SECURITY_NATIVE_DREP, nullptr, 0, &contextHandle, &OutBuffer, &sspiOutFlags, &tsExpiry);
		if (FAILED(status))
			throw std::runtime_error("InitializeSecurityContext failed");
	}
	else
	{
		status = AcceptSecurityContext(&credHandle, &contextHandle, nullptr, sspiServerFlags, 0, nullptr, &OutBuffer, &sspiOutFlags, &tsExpiry);
		if (FAILED(status))
			throw std::runtime_error("AcceptSecurityContext failed");
	}

	if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer)
	{
		sendBuffers.push_back(OutBuffers[0]);
	}
}

void TlsStreamWin32::loadPfxCredentials(const void* data, size_t size, const wchar_t* password)
{
	CertHandle cert = nullptr;
	HCERTSTORE store = nullptr;

	try
	{
		CRYPT_DATA_BLOB pfx = {};
		pfx.pbData = (BYTE*)data;
		pfx.cbData = (DWORD)size;
		store = PFXImportCertStore(&pfx, password, 0);
		if (store == nullptr)
			throw std::runtime_error("PFXImportCertStore failed");

		cert = CertFindCertificateInStore(store, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_HAS_PRIVATE_KEY, nullptr, nullptr);
		if (cert == nullptr)
			throw std::runtime_error("No certificate with private key found");

		/*
		HCERTSTORE memstore = CertOpenStore(CERT_STORE_PROV_MEMORY, 0, 0, 0, nullptr);
		CertHandle certcopy = 0;
		if (!CertAddCertificateContextToStore(memstore, cert, CERT_STORE_ADD_REPLACE_EXISTING, &certcopy))
			throw std::runtime_error("CertAddCertificateContextToStore failed");
		CertFreeCertificateContext(cert);
		CertCloseStore(memstore, 0);
		cert = certcopy;
		*/

		/*
		BOOL bFreeHandle = FALSE;
		HCRYPTPROV hProv = 0;
		DWORD dwKeySpec = 0;
		BOOL bResult = CryptAcquireCertificatePrivateKey(cert, 0, nullptr, &hProv, &dwKeySpec, &bFreeHandle);
		if (bResult && bFreeHandle && dwKeySpec == CERT_NCRYPT_KEY_SPEC) NCryptFreeObject(hProv);
		else if (bResult && bFreeHandle) CryptReleaseContext(hProv, 0);
		*/

		// CertAddCertificateContextToStore(certstore, cert, CERT_STORE_ADD_REPLACE_EXISTING, &certStoreContext);

		SCHANNEL_CRED schannelCred = { 0 };
		schannelCred.dwVersion = SCHANNEL_CRED_VERSION;
		schannelCred.grbitEnabledProtocols = SP_PROT_NONE; // SP_PROT_TLS1_3 | SP_PROT_TLS1_2 | SP_PROT_TLS1_1 | SP_PROT_TLS1_0;
		schannelCred.cCreds = 1;
		schannelCred.paCred = &cert;
		schannelCred.dwFlags = SCH_USE_STRONG_CRYPTO;

		std::wstring namebuffer = UNISP_NAME;
		TimeStamp tsExpiry;
		SECURITY_STATUS result = AcquireCredentialsHandle(nullptr, (LPWSTR)namebuffer.c_str(), client ? SECPKG_CRED_OUTBOUND : SECPKG_CRED_INBOUND, nullptr, &schannelCred, nullptr, nullptr, &credHandle, &tsExpiry);
		if (result != SEC_E_OK)
			throw std::runtime_error("AcquireCredentialsHandle failed with error code " + std::to_string(result));

		freeCertificate(cert);
		CertCloseStore(store, 0);
	}
	catch (...)
	{
		if (store)
			CertCloseStore(store, 0);
		if (cert)
			freeCertificate(cert);
		throw;
	}
}

void TlsStreamWin32::createCredentials()
{
	CertHandle cert = nullptr;
	SCHANNEL_CRED schannelCred = { 0 };
	schannelCred.dwVersion = SCHANNEL_CRED_VERSION;
	schannelCred.grbitEnabledProtocols = SP_PROT_NONE; // SP_PROT_TLS1_3 | SP_PROT_TLS1_2 | SP_PROT_TLS1_1 | SP_PROT_TLS1_0;

	try
	{
		if (client)
		{
			schannelCred.dwFlags = SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_MANUAL_CRED_VALIDATION | SCH_CRED_NO_SERVERNAME_CHECK | SCH_USE_STRONG_CRYPTO;
		}
		else
		{
			std::wstring x500 = L"CN=lasernet";
			DWORD x500EncodedLength = 0;
			CertStrToName(X509_ASN_ENCODING, x500.c_str(), CERT_X500_NAME_STR, nullptr, nullptr, &x500EncodedLength, nullptr);
			std::vector<BYTE> x500Encoding(x500EncodedLength);
			CertStrToName(X509_ASN_ENCODING, x500.c_str(), CERT_X500_NAME_STR, nullptr, x500Encoding.data(), &x500EncodedLength, nullptr);

			CERT_NAME_BLOB subjectIssuerBlob;
			memset(&subjectIssuerBlob, 0, sizeof(subjectIssuerBlob));
			subjectIssuerBlob.cbData = (DWORD)x500Encoding.size();
			subjectIssuerBlob.pbData = x500Encoding.data();

			SYSTEMTIME startTime, endTime;
			GetSystemTime(&startTime);
			GetSystemTime(&endTime);
			startTime.wYear -= 4;
			endTime.wYear += 4;

			cert = CertCreateSelfSignCertificate(0, &subjectIssuerBlob, 0, nullptr, nullptr, &startTime, &endTime, 0);
			if (cert == nullptr)
				throw std::runtime_error("Failed to create self-signed certificate for TLS connection");

			schannelCred.cCreds = 1;
			schannelCred.paCred = &cert;
			schannelCred.dwFlags = SCH_USE_STRONG_CRYPTO;
		}

		std::wstring namebuffer = UNISP_NAME;
		TimeStamp tsExpiry;
		SECURITY_STATUS result = AcquireCredentialsHandle(nullptr, (LPWSTR)namebuffer.c_str(), client ? SECPKG_CRED_OUTBOUND : SECPKG_CRED_INBOUND, nullptr, &schannelCred, nullptr, nullptr, &credHandle, &tsExpiry);
		if (result != SEC_E_OK)
			throw std::runtime_error("AcquireCredentialsHandle failed with error code " + std::to_string(result));

		freeCertificate(cert);
	}
	catch (...)
	{
		freeCertificate(cert);
		throw;
	}
}

void TlsStreamWin32::freeCertificate(CertHandle context)
{
	if (context == nullptr)
		return;

	DWORD len = 0;
	CertGetCertificateContextProperty(context, CERT_KEY_PROV_INFO_PROP_ID, nullptr, &len);
	std::vector<char> buffer(len);
	if (CertGetCertificateContextProperty(context, CERT_KEY_PROV_INFO_PROP_ID, buffer.data(), &len))
	{
		const CRYPT_KEY_PROV_INFO* keyProvInfo = reinterpret_cast<const CRYPT_KEY_PROV_INFO*>(buffer.data());
		HCRYPTPROV dummy;
		BOOL result = CryptAcquireContext(&dummy, keyProvInfo->pwszContainerName, keyProvInfo->pwszProvName, keyProvInfo->dwProvType, CRYPT_DELETEKEYSET);
		if (!result)
			result = CryptAcquireContext(&dummy, keyProvInfo->pwszContainerName, keyProvInfo->pwszProvName, keyProvInfo->dwProvType, CRYPT_DELETEKEYSET | CRYPT_MACHINE_KEYSET);
	}
	CertFreeCertificateContext(context);
}

std::wstring TlsStreamWin32::to_utf16(const std::string& str)
{
	if (str.empty()) return {};
	int needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
	if (needed == 0)
		throw std::runtime_error("MultiByteToWideChar failed");
	std::wstring result;
	result.resize(needed);
	needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &result[0], (int)result.size());
	if (needed == 0)
		throw std::runtime_error("MultiByteToWideChar failed");
	return result;
}
