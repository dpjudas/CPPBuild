
#include "Precomp.h"
#include "HttpConnectionImpl.h"

std::unique_ptr<HttpConnection> HttpConnection::create()
{
	return std::make_unique<HttpConnectionImpl>();
}
