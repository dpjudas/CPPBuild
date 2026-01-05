
#include "Precomp.h"
#include "TcpSocket.h"
#include "SocketAddress.h"
#include <stdexcept>

void TcpSocket::init()
{
#ifdef WIN32
	static struct InitWinsock
	{
		InitWinsock()
		{
			WSADATA wsaData = {};
			int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (result != 0)
				throw std::runtime_error("WSAStartup failed");
		}
		~InitWinsock()
		{
			WSACleanup();
		}
	} initwinsock;
#endif
}

TcpSocket::TcpSocket()
{
	m_handle = socket(AF_INET, SOCK_STREAM, 0);
	if (m_handle == INVALID_SOCKET)
		throw std::runtime_error("Unable to create socket handle");
}

TcpSocket::TcpSocket(SOCKET handle) : m_handle(handle)
{
}

TcpSocket::~TcpSocket()
{
	closesocket(m_handle);
	m_handle = INVALID_SOCKET;
}

void TcpSocket::setNonblocking(bool enable)
{
	u_long value = enable;
	ioctlsocket(m_handle, FIONBIO, &value);
}

void TcpSocket::setTcpNoDelay(bool enable)
{
	int value = enable;
	int result = setsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(int));
	if (result == SOCKET_ERROR)
		throw std::runtime_error("Unable to set TCP_NODELAY on socket");
}

void TcpSocket::setReuseAddr(bool enable)
{
	int value = enable;
	int result = setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&value, sizeof(int));
	if (result == SOCKET_ERROR)
		throw std::runtime_error("Unable to set SO_REUSEADDR on socket");
}

void TcpSocket::bind(const SocketAddress& endpoint)
{
	sockaddr_in addr;
	endpoint.toSockaddr(AF_INET, (sockaddr*)&addr, sizeof(sockaddr_in));
	int result = ::bind(m_handle, (const sockaddr*)&addr, sizeof(sockaddr_in));
	if (result == SOCKET_ERROR)
		throw std::runtime_error("Unable to bind socket");
}

void TcpSocket::listen(int backlog)
{
	int result = ::listen(m_handle, backlog);
	if (result == SOCKET_ERROR)
		throw std::runtime_error("Unable to listen on socket");
}

std::unique_ptr<TcpSocket> TcpSocket::accept()
{
	sockaddr_in new_addr = { 0 };
	int size = sizeof(sockaddr_in);
	SOCKET accepted_socket = ::accept(m_handle, (sockaddr*)&new_addr, &size);
	if (accepted_socket == INVALID_SOCKET)
		throw std::runtime_error("Socket accept failed");
	return std::make_unique<TcpSocket>(accepted_socket);
}

void TcpSocket::connect(const SocketAddress& endpoint)
{
	sockaddr_in addr;
	endpoint.toSockaddr(AF_INET, (sockaddr*)&addr, sizeof(sockaddr_in));
	int result = ::connect(m_handle, (const sockaddr*)&addr, sizeof(sockaddr_in));
	if (result != SOCKET_ERROR)
		return;

	int errorcode = WSAGetLastError();
	if (errorcode != WSAEWOULDBLOCK && errorcode != WSAEINPROGRESS)
		throw std::runtime_error("Socket unable to connect");
}

int TcpSocket::send(const void* data, int size)
{
	int result = ::send(m_handle, static_cast<const char*>(data), size, 0);
	if (result != SOCKET_ERROR)
		return result;

	int errorcode = WSAGetLastError();
	if (errorcode == WSAEWOULDBLOCK)
		return -1;
	else if (errorcode == WSAECONNRESET)
		throw std::runtime_error("Connection reset by the remote host");
	else
		throw std::runtime_error("Socket send failed");
}

int TcpSocket::receive(void* data, int size)
{
	int result = ::recv(m_handle, static_cast<char*>(data), size, 0);
	if (result != SOCKET_ERROR)
		return result;

	int errorcode = WSAGetLastError();
	if (errorcode == WSAEWOULDBLOCK)
		return -1;
	else if (errorcode == WSAECONNRESET)
		throw std::runtime_error("Connection reset by the remote host");
	else
		throw std::runtime_error("Socket receive failed");
}

void TcpSocket::shutdownSend()
{
	::shutdown(m_handle, SD_SEND);
}
