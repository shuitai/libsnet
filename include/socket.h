#ifndef _BASE_SOCKET_H_
#define _BASE_SOCKET_H_

#include "sigslot.h"
#include "common.h"
#include "socketaddress.h"
#include "eventloop.h"

enum SocketType 
{
	NET_SOCK_STREAM,
	NET_SOCK_DGRAM
};

class Socket
{
public:
	virtual ~Socket (void) {}
	static Socket* CreateSocket(SocketType socketType, const EventLoop *loop = NULL);
	
	virtual int Bind(const SocketAddress& bindAddress) = 0;
	virtual int Connect(const SocketAddress& address) = 0;
	virtual int Send(const char* buf, size_t size) = 0;
	virtual int SendTo(const char* buf, size_t size, const SocketAddress& address) = 0;
	virtual void Close() = 0;

	sigslot::signal2<Socket*, int> SignalConnectEvent;     // connected
	sigslot::signal2<Socket*, int> SignalWriteEvent;       // ready to write
	sigslot::signal4<Socket*, const char*, size_t, const SocketAddress&> SignalReadEvent;        // ready to read
	sigslot::signal1<Socket*> SignalCloseEvent;  // closed
	sigslot::signal2<Socket*, int> SignalErrorEvent; // error
};

class ServerSocket
{
public:
	virtual ~ServerSocket() {}

	static ServerSocket* CreateServerSocket(SocketType socketType, const EventLoop *loop = NULL);

	virtual int Bind(const SocketAddress& bindAddress) = 0;
	virtual int Listen() = 0;
	virtual void Close() = 0;

	sigslot::signal2<ServerSocket*, Socket*> SignalAcceptEvent; // Accept event
	sigslot::signal1<ServerSocket*> SignalCloseEvent;  // closed
	sigslot::signal1<ServerSocket*> SignalErrorEvent;
};

#endif