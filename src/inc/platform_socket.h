#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS 1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

using socket_t = SOCKET;
#define IS_VALID_SOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSE_SOCKET(s)    closesocket(s)
#define GET_SOCKET_ERR()   WSAGetLastError()
#define ERR_EWOULDBLOCK    WSAEWOULDBLOCK
#define ERR_ECONNRESET     WSAECONNRESET

#else
#define PLATFORM_LINUX 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

using socket_t = int;
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)
#define IS_VALID_SOCKET(s) ((s) >= 0)
#define CLOSE_SOCKET(s)    close(s)
#define GET_SOCKET_ERR()   errno
#define ERR_EWOULDBLOCK    EWOULDBLOCK
#define ERR_ECONNRESET     ECONNRESET
#endif

// Cross-platform Socket Initialization & Cleanup helpers
inline bool InitializeSockets() {
#if defined(PLATFORM_WINDOWS)
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
	return true; // No initialization needed on POSIX
#endif
}

inline void CleanupSockets() {
#if defined(PLATFORM_WINDOWS)
	WSACleanup();
#endif
}

// Cross-platform non-blocking mode setting
inline bool SetNonBlocking(socket_t sock, bool enable) {
#if defined(PLATFORM_WINDOWS)
	u_long mode = enable ? 1 : 0;
	return ioctlsocket(sock, FIONBIO, &mode) == 0;
#else
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags == -1) return false;
	flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
	return fcntl(sock, F_SETFL, flags) == 0;
#endif
}