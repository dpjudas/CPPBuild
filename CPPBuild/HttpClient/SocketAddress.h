#pragma once

#include <string>

class SocketAddress
{
public:
	SocketAddress(const std::string& host, int port);
	SocketAddress(int port);
	SocketAddress(int domain, struct sockaddr* addr, int len);

	void toSockaddr(int domain, struct sockaddr* out_addr, int len) const;

	std::string host;
	int port = 0;
};
