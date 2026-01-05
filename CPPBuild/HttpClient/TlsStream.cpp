
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

std::unique_ptr<TlsStream> TlsStream::create(ByteStream* socketStream)
{
	throw std::runtime_error("TlsStream not implemented");
}

#endif
