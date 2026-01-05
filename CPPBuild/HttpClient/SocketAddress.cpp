
#include "Precomp.h"
#include "SocketAddress.h"
#include <stdexcept>
#include <string>

#ifdef WIN32
typedef unsigned long in_addr_t;
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

SocketAddress::SocketAddress(const std::string& host, int port) : host(host), port(port)
{
}

SocketAddress::SocketAddress(int port) : host("*"), port(port)
{
}

SocketAddress::SocketAddress(int domain, struct sockaddr* addr, int len)
{
	if (domain != AF_INET)
		throw std::runtime_error("Only AF_INET domain supported");

	if (len < sizeof(sockaddr_in))
		throw std::runtime_error("Insufficient buffer for sockaddr_in structure");

	sockaddr_in* addr_in = (sockaddr_in*)addr;
	if (addr_in->sin_family != AF_INET)
		throw std::runtime_error("Unexpected sin_family in sockaddr");

	unsigned long addr_long = (unsigned long)ntohl(addr_in->sin_addr.s_addr);
	port = ntohs(addr_in->sin_port);

	if (addr_in->sin_addr.s_addr == INADDR_ANY)
	{
		host = "*";
	}
	else
	{
		host =
			std::to_string(static_cast<int>((addr_long & 0xff000000) >> 24)) + "." +
			std::to_string(static_cast<int>((addr_long & 0x00ff0000) >> 16)) + "." +
			std::to_string(static_cast<int>((addr_long & 0x0000ff00) >> 8)) + "." +
			std::to_string(static_cast<int>((addr_long & 0x000000ff)));
	}
}

void SocketAddress::toSockaddr(int domain, struct sockaddr* out_addr, int len) const
{
	if (domain != AF_INET)
		throw std::runtime_error("Only AF_INET domain supported");

	if (len < sizeof(sockaddr_in))
		throw std::runtime_error("Insufficient buffer for sockaddr_in structure");

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (host.empty() || host == "*")
	{
		addr.sin_addr.s_addr = INADDR_ANY;
	}
	else if (InetPtonA(AF_INET, host.c_str(), &addr.sin_addr) <= 0) // 1 = success, 0 = invalid ip address, -1 = error
	{
		ADDRINFOA hints = { 0 };
		hints.ai_family = AF_INET;

		PADDRINFOA result;
		int lookupResult = GetAddrInfoA(host.c_str(), std::to_string(port).c_str(), &hints, &result);
		if (lookupResult == ERROR_SUCCESS)
		{
			for (PADDRINFOA current = result; current != nullptr; current = current->ai_next)
			{
				if (current->ai_family == AF_INET)
				{
					addr = *reinterpret_cast<sockaddr_in*>(current->ai_addr);
					break;
				}
			}
		}
		else
		{
			throw std::runtime_error("Host not found");
		}
	}

	memcpy(out_addr, &addr, sizeof(sockaddr_in));
}
