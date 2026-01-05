
#include "Precomp.h"
#include "SocketStream.h"
#include "TcpSocket.h"

SocketStream::SocketStream(const std::string& host, int port)
{
	sock = std::make_unique<TcpSocket>();
	sock->setTcpNoDelay(true);
	sock->connect(SocketAddress(host, port));
}

void SocketStream::write(const void* data, size_t size, WriteCompleteCallback writeComplete)
{
	writeState.data = (const uint8_t*)data;
	writeState.size = size;
	writeState.writeComplete = writeComplete;
}

void SocketStream::read(void* data, size_t size, ReadCompleteCallback readComplete)
{
	readState.data = (uint8_t*)data;
	readState.size = size;
	readState.readComplete = readComplete;
}

void SocketStream::process()
{
	SocketSelect ss;
	// ss.setTimeout(5, 0);

	bool done = true;
	if (readState.size != 0)
	{
		ss.setRead(*sock);
		done = false;
	}
	if (writeState.size != 0)
	{
		ss.setWrite(*sock);
		ss.setExcept(*sock);
		done = false;
	}

	if (done)
		throw std::runtime_error("Nothing to read or write to socket");

	ss.select();

	if (readState.size && ss.isSetRead(*sock))
	{
		int bytes = sock->receive(readState.data, (int)readState.size);
		if (bytes >= 0)
		{
			auto readComplete = std::move(readState.readComplete);
			readState.data = nullptr;
			readState.size = 0;
			readState.readComplete = {};
			readComplete(bytes);
		}
	}

	if (writeState.size && ss.isSetWrite(*sock))
	{
		int bytes = sock->send(writeState.data, (int)writeState.size);
		if (bytes >= 0)
		{
			auto writeComplete = std::move(writeState.writeComplete);
			writeState.data = nullptr;
			writeState.size = 0;
			writeState.writeComplete = {};
			writeComplete(bytes);
		}
	}
	else if (writeState.size && ss.isSetExcept(*sock))
	{
		throw std::exception("Unable to connect");
	}
}
