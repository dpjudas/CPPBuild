#pragma once

#include "ByteStream.h"
#include "TcpSocket.h"

class SocketStream : public ByteStream
{
public:
	SocketStream(const std::string& host, int port);

	void write(const void* data, size_t size, WriteCompleteCallback writeComplete) override;
	void read(void* data, size_t size, ReadCompleteCallback readComplete) override;

	void process();

private:
	std::unique_ptr<TcpSocket> sock;

	struct
	{
		const uint8_t* data = nullptr;
		size_t size = 0;
		WriteCompleteCallback writeComplete = {};
	} writeState;

	struct
	{
		uint8_t* data = nullptr;
		size_t size = 0;
		ReadCompleteCallback readComplete = {};
	} readState;
};
