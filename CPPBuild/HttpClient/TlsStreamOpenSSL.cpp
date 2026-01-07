
#include "Precomp.h"
#include "TlsStreamOpenSSL.h"

TlsStreamOpenSSL::TlsStreamOpenSSL(ByteStream* socketStream) : socketStream(socketStream)
{
}

TlsStreamOpenSSL::~TlsStreamOpenSSL()
{
}

void TlsStreamOpenSSL::authenticateAsServer(AuthCompleteCallback authComplete, const void* pfxData, size_t pfxSize, const char* pfxPassword)
{
}

void TlsStreamOpenSSL::authenticateAsClient(AuthCompleteCallback authComplete, const std::string& targetName)
{
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
