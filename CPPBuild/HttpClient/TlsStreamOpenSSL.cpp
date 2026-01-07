
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

	// see https://github.com/openssl/openssl/blob/master/demos/guide/tls-client-non-block.c
	// bio = create_socket_bio(hostname, port, AF_INET);
	if (!bio)
		throw std::runtime_error("Could not create BIO object");
	SSL_set_bio(ssl, bio, bio);

	// pass hostname to server in case it has multiple certifcates based on the hostname
	if (!SSL_set_tlsext_host_name(ssl, targetName.c_str()))
		throw std::runtime_error("Could not set SNI hostname");

	// verify the SNI hostname matches the certificate
	if (!SSL_set1_host(ssl, targetName.c_str()))
		throw std::runtime_error("Could not enable hostname verification");

	// SSL_connect(ssl);
	// SSL_write_ex(ssl, data, len, byteswritten);
	// SSL_read_ex(ssl, data, size, bytesread);
	// SSL_shutdown(ssl);

	// SSL_get_error(ssl, res);
	// SSL_ERROR_WANT_READ
	// SSL_ERROR_WANT_WRITE
	// SSL_ERROR_ZERO_RETURN (EOF)
	// SSL_ERROR_SYSCALL (failure)
	// SSL_ERROR_SSL:
	//	if (SSL_get_verify_result(ssl) != X509_V_OK)
	//		throw std::runtime_error(X509_verify_cert_error_string(SSL_get_verify_result(ssl)));
}

void TlsStreamOpenSSL::write(const void* data, size_t size, WriteCompleteCallback writeComplete)
{
}

void TlsStreamOpenSSL::read(void* data, size_t size, ReadCompleteCallback readComplete)
{
}

void TlsStreamOpenSSL::shutdown(ShutdownCompleteCallback shutdownComplete)
{
}
