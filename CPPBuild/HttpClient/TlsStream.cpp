
#include "Precomp.h"
#include "TlsStream.h"
#include <stdexcept>

#ifdef WIN32
#include "TlsStreamWin32.h"

std::unique_ptr<TlsStream> TlsStream::create(ByteStream* socketStream)
{
	return std::make_unique<TlsStreamWin32>(socketStream);
}

#else
#include "TlsStreamOpenSSL.h"

std::unique_ptr<TlsStream> TlsStream::create(ByteStream* socketStream)
{
	return std::make_unique<TlsStreamOpenSSL>(socketStream);
}

#endif
