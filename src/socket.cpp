#include "socket.h"
#include "socket-tcp.h"
#include "socket-udp.h"

Socket* Socket::CreateSocket(SocketType socketType, const EventLoop *loop)
{
	Socket* sock = NULL;
	if (socketType == NET_SOCK_STREAM)
	{
		if (loop == NULL)
		{
			sock = new TCPSocket(EventLoopAdapter::Instance());
		}
		else
		{
			sock = new TCPSocket((EventLoopAdapter*)loop);
		}
		
	}
	else if (socketType == NET_SOCK_DGRAM)
	{
		if (loop == NULL)
		{
			sock = new UDPSocket(EventLoopAdapter::Instance());
		}
		else
		{
			sock = new UDPSocket((EventLoopAdapter*)loop);
		}
	}

	return sock;
}

ServerSocket* ServerSocket::CreateServerSocket(SocketType socketType, const EventLoop* loop)
{
	ServerSocket* serverSock = NULL;

	if (loop == NULL)
	{
		serverSock = new TCPServerSocket(EventLoopAdapter::Instance());
	}
	else
	{
		serverSock = new TCPServerSocket((EventLoopAdapter*)loop);
	}
	
	return serverSock;
}