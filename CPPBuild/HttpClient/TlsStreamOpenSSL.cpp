
#include "Precomp.h"
#include "TlsStreamOpenSSL.h"
#include <stdexcept>

TlsStreamOpenSSL::TlsStreamOpenSSL(ByteStream* socketStream) : socketStream(socketStream)
{
}

TlsStreamOpenSSL::~TlsStreamOpenSSL()
{
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	BIO_free(network_bio);
}

void TlsStreamOpenSSL::authenticateAsServer(AuthCompleteCallback authCompleteCallback, const void* pfxData, size_t pfxSize, const char* pfxPassword)
{
	throw std::runtime_error("authenticateAsServer not implemented");
}

void TlsStreamOpenSSL::authenticateAsClient(AuthCompleteCallback authCompleteCallback, const std::string& targetName)
{
	ctx = SSL_CTX_new(TLS_client_method());
	if (!ctx)
		throw std::runtime_error("Could not create SSL context");

	// abort the handshake if certificate verification fails
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);

	// use the default trusted certificate store
	if (!SSL_CTX_set_default_verify_paths(ctx))
		throw std::runtime_error("Could not set the default trusted certificate store");

	// reject too old versions of TLS
	if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
		throw std::runtime_error("Could not set minimum TLS protocol version");

	ssl = SSL_new(ctx);
	if (!ssl)
		throw std::runtime_error("Could not create SSL object");

	int result = BIO_new_bio_pair(&internal_bio, 0, &network_bio, 0);
	if (result)
		throw std::runtime_error("Could not create BIO pair");
	SSL_set_bio(ssl, internal_bio, internal_bio); // note: ssl object takes ownership of internal_bio

	// pass hostname to server in case it has multiple certifcates based on the hostname
	if (!SSL_set_tlsext_host_name(ssl, targetName.c_str()))
		throw std::runtime_error("Could not set SNI hostname");

	// verify the SNI hostname matches the certificate
	if (!SSL_set1_host(ssl, targetName.c_str()))
		throw std::runtime_error("Could not enable hostname verification");

	state = TlsStreamState::Handshake;
	authComplete = authCompleteCallback;
	process();
}

void TlsStreamOpenSSL::write(const void* data, size_t size, WriteCompleteCallback writeCompleteCallback)
{
	if (state != TlsStreamState::Open)
		throw std::runtime_error("TLS session not open");

	writeState.data = (uint8_t*)data;
	writeState.size = size;
	writeState.writeComplete = writeCompleteCallback;
	process();
}

void TlsStreamOpenSSL::read(void* data, size_t size, ReadCompleteCallback readCompleteCallback)
{
	if (state != TlsStreamState::Open)
		throw std::runtime_error("TLS session not open");

	readState.data = (uint8_t*)data;
	readState.size = size;
	readState.readComplete = readCompleteCallback;
	process();
}

void TlsStreamOpenSSL::shutdown(ShutdownCompleteCallback shutdownCompleteCallback)
{
	if (state != TlsStreamState::Open)
		return;

	state = TlsStreamState::Shutdown;
	shutdownComplete = shutdownCompleteCallback;
	process();
}

void TlsStreamOpenSSL::startReadSocket()
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

void TlsStreamOpenSSL::readSocketComplete(size_t bytesRead)
{
	readSocketOpen = (bytesRead != 0);
	readBufferAvailable += bytesRead;
	readSocketActive = false;
	process();
}

void TlsStreamOpenSSL::startWriteSocket()
{
	if (writeSocketActive)
		return;

	if (writeBuffer.size() < 0x10000)
		writeBuffer.resize(0x10000);

	if (writeBufferPos == writeBufferSize)
	{
		size_t bytesread = 0;
		int result = BIO_read_ex(network_bio, writeBuffer.data(), writeBuffer.size());
		if (result != 1)
			throw std::runtime_error("BIO_read_ex failed");
		writeBufferPos = 0;
		writeBufferSize = bytesread;
	}

	if (writeBufferPos < writeBufferSize)
	{
		writeSocketActive = true;
		socketStream->write(writeBuffer.data() + writeBufferPos, writeBufferSize - writeBufferPos, [this](size_t bytesWritten) { writeSocketComplete(bytesWritten); });
	}
}

void TlsStreamOpenSSL::writeSocketComplete(size_t bytesWritten)
{
	writeBufferPos += bytesWritten;
	writeSocketActive = false;
	process();
}

void TlsStreamOpenSSL::process()
{
	if (readBufferPos < readBufferAvailable)
	{
		size_t written = 0;
		int result = BIO_write_ex(network_bio, readBuffer.data() + readBufferPos, readBufferAvailable - readBufferPos, &written);
		if (result == 1)
		{
			readBufferPos += written;
			if (readBufferPos == readBufferAvailable)
			{
				readBufferPos = 0;
				readBufferAvailable = 0;
			}
		}
	}

	if (state == TlsStreamState::Handshake)
	{
		int result = SSL_connect(ssl);
		if (result == 1)
		{
			state = TlsStreamState::Open;
			authComplete();
		}
		else
		{
			handleIOFailure(result);
		}
	}
	else if (state == TlsStreamState::Open)
	{
		if (writeState.size > 0)
		{
			size_t written = 0;
			int result = SSL_write_ex(ssl, writeState.data, writeState.size, &written);
			if (result == 0)
			{
				writeState.size = 0;
				writeState.writeComplete(written);
				startWriteSocket(); // flush to socket
			}
			else
			{
				handleIOFailure(result);
			}
		}

		if (readState.size > 0)
		{
			size_t readbytes = 0;
			int result = SSL_read_ex(ssl, readState.data, readState.size, &readbytes);
			if (result == 0)
			{
				readState.size = 0;
				readState.readComplete(readbytes);
			}
			else if (handleIOFailure(result))
			{
				readState.size = 0;
				readState.readComplete(0);
			}
		}
	}
	else if (state == TlsStreamState::Shutdown)
	{
		int result = SSL_shutdown(ssl);
		if (result >= 0)
		{
			state = TlsStreamState::Closed;
			shutdownComplete();
			startWriteSocket(); // flush to socket
		}
		else
		{
			handleIOFailure(result);
		}
	}
}

bool TlsStreamOpenSSL::handleIOFailure(int result)
{
	int error = SSL_get_error(ssl, result);
	if (error == SSL_ERROR_NONE)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_NONE");
	}
	else if (error == SSL_ERROR_ZERO_RETURN)
	{
		// EOF received
		return true;
	}
	else if (error == SSL_ERROR_WANT_READ)
	{
		startWriteSocket(); // flush pending writes before read
		startReadSocket();
		return false;
	}
	else if (error == SSL_ERROR_WANT_WRITE)
	{
		startWriteSocket();
		return false;
	}
	else if (error == SSL_ERROR_WANT_CONNECT)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_CONNECT");
	}
	else if (error == SSL_ERROR_WANT_ACCEPT)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_ACCEPT");
	}
	else if (error == SSL_ERROR_WANT_X509_LOOKUP)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_X509_LOOKUP");
	}
	else if (error == SSL_ERROR_WANT_ASYNC)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_ASYNC");
	}
	else if (error == SSL_ERROR_WANT_ASYNC_JOB)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_ASYNC_JOB");
	}
	else if (error == SSL_ERROR_WANT_CLIENT_HELLO_CB)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_CLIENT_HELLO_CB");
	}
	else if (error == SSL_ERROR_SYSCALL)
	{
		throw std::runtime_error("Fatal I/O error (SSL_ERROR_SYSCALL)");
	}
	else if (error == SSL_ERROR_SSL)
	{
		if (SSL_get_verify_result(ssl) != X509_V_OK)
			throw std::runtime_error(X509_verify_cert_error_string(SSL_get_verify_result(ssl)));
		throw std::runtime_error("SSL error");
	}
	else
	{
		throw std::runtime_error("Unknown SSL error code " + std::to_string(error));
	}
}
