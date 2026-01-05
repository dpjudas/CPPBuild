#pragma once

#ifdef WIN32

#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <WinSock2.h>
#include <Windows.h>

#else

#include <sys/socket.h>

typedef int SOCKET;
#define INVALID_SOCKET (SOCKET)(-1)
#define SOCKET_ERROR (-1)

#endif
