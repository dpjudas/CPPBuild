
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

void TlsStreamOpenSSL::authenticateAsServer(AuthCompleteCallback authComplete, const void* pfxData, size_t pfxSize, const char* pfxPassword)
{
	throw std::runtime_error("authenticateAsServer not implemented");
}

void TlsStreamOpenSSL::authenticateAsClient(AuthCompleteCallback authComplete, const std::string& targetName)
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

	result = SSL_connect(ssl);
	if (result == 1)
		authComplete();
	else
		processIO(result);

	// see https://github.com/openssl/openssl/blob/master/demos/guide/tls-client-non-block.c
	// BIO_ctrl_pending
	// BIO_ctrl_wpending
	// BIO_get_write_guarantee (macro)
	// BIO_ctrl_get_write_guarantee (function)
	// BIO_get_read_request
	// BIO_ctrl_get_read_request
	// BIO_pending
	// BIO_write_ex
	// BIO_read_ex
	// BIO_should_read
	// BIO_eof
}

void TlsStreamOpenSSL::write(const void* data, size_t size, WriteCompleteCallback writeComplete)
{
	size_t written = 0;
	int result = SSL_write_ex(ssl, data, size, &written);
	if (result == 0)
		writeComplete(written);
	else
		processIO(result);
}

void TlsStreamOpenSSL::read(void* data, size_t size, ReadCompleteCallback readComplete)
{
	size_t readbytes = 0;
	int result = SSL_read_ex(ssl, data, size, &readbytes);
	if (result == 0)
		readComplete(readbytes);
	else
		processIO(result);
}

void TlsStreamOpenSSL::shutdown(ShutdownCompleteCallback shutdownComplete)
{
	// To do: do we need to call BIO_shutdown_wr too?
	int result = SSL_shutdown(ssl);
	if (result >= 0)
		shutdownComplete();
	else
		processIO(result);
}

void TlsStreamOpenSSL::startSocketRead()
{
}

void TlsStreamOpenSSL::startSocketWrite()
{
}

bool TlsStreamOpenSSL::processIO(int result)
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
		startSocketRead();
		return false;
	}
	else if (error == SSL_ERROR_WANT_WRITE)
	{
		startSocketWrite();
		return false;
	}
	else if (error == SSL_ERROR_WANT_CONNECT)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_CONNECT received");
	}
	else if (error == SSL_ERROR_WANT_ACCEPT)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_ACCEPT received");
	}
	else if (error == SSL_ERROR_WANT_X509_LOOKUP)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_X509_LOOKUP received");
	}
	else if (error == SSL_ERROR_WANT_ASYNC)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_ASYNC received");
	}
	else if (error == SSL_ERROR_WANT_ASYNC_JOB)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_ASYNC_JOB received");
	}
	else if (error == SSL_ERROR_WANT_CLIENT_HELLO_CB)
	{
		throw std::runtime_error("Unexpected SSL_ERROR_WANT_CLIENT_HELLO_CB received");
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
