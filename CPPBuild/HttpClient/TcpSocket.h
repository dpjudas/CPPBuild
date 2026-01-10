#pragma once

#include "SocketAddress.h"
#include "SocketHeaders.h"
#include <stdexcept>
#include <memory>

class TcpSocket
{
public:
	TcpSocket();
	TcpSocket(SOCKET handle);
	~TcpSocket();

	void setNonblocking(bool enable = true);
	void setTcpNoDelay(bool enable = true);
	void setReuseAddr(bool enable = true);

	void bind(const SocketAddress& endpoint);
	void listen(int backlog);
	std::unique_ptr<TcpSocket> accept();

	void connect(const SocketAddress& endpoint);

	int send(const void* data, int size);
	int receive(void* data, int size);

	void shutdownSend();

	SOCKET handle() const { return m_handle; }

	static void init();

private:
	TcpSocket(const TcpSocket&) = delete;
	TcpSocket& operator=(const TcpSocket&) = delete;

	SOCKET m_handle = INVALID_SOCKET;
};

class SocketSelect
{
public:
	SocketSelect()
	{
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
	}

	void setTimeout(int sec, int usec) { timeout.tv_sec = sec; timeout.tv_usec = usec; timeout_set = true; }

	void setRead(const TcpSocket& socket) { SOCKET handle = socket.handle(); FD_SET(handle, &rfds); rfds_set = true; maxfds = std::max(maxfds, handle); }
	void setWrite(const TcpSocket& socket) { SOCKET handle = socket.handle(); FD_SET(handle, &wfds); wfds_set = true; maxfds = std::max(maxfds, handle); }
	void setExcept(const TcpSocket& socket) { SOCKET handle = socket.handle(); FD_SET(handle, &efds); efds_set = true; maxfds = std::max(maxfds, handle); }

	bool select()
	{
#ifdef WIN32
		int result = ::select(0, rfds_set ? &rfds : nullptr, wfds_set ? &wfds : nullptr, efds_set ? &efds : nullptr, timeout_set ? &timeout : nullptr);
#else
		int result = ::select(maxfds + 1, rfds_set ? &rfds : nullptr, wfds_set ? &wfds : nullptr, efds_set ? &efds : nullptr, timeout_set ? &timeout : nullptr);
#endif
		if (result == SOCKET_ERROR)
			throw std::runtime_error("Socket select failed");
		return result > 0;
	}

	bool isSetRead(const TcpSocket& socket) const { SOCKET handle = socket.handle(); return FD_ISSET(handle, &rfds) != 0; }
	bool isSetWrite(const TcpSocket& socket) const { SOCKET handle = socket.handle(); return FD_ISSET(handle, &wfds) != 0; }
	bool isSetExcept(const TcpSocket& socket) const { SOCKET handle = socket.handle(); return FD_ISSET(handle, &efds) != 0; }

private:
	bool rfds_set = false;
	bool wfds_set = false;
	bool efds_set = false;
	bool timeout_set = false;
	SOCKET maxfds = INVALID_SOCKET;
	fd_set rfds, wfds, efds;
	timeval timeout = {};
};
