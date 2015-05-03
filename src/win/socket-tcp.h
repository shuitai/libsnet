#ifndef _BASE_SOCKET_TCP_IMPL_H_
#define _BASE_SOCKET_TCP_IMPL_H_

#include "socket-inl.h"
#include "eventloopadapter.h"
#include <MSWSock.h>

class TCPSocket : public Socket
{
public:
	TCPSocket(EventLoopAdapter* eventLoop);
	TCPSocket(EventLoopAdapter* eventLoop, sockfd_t incomeSock, const SocketAddress& address);
	
	~TCPSocket();
	virtual int Bind(const SocketAddress& address);
	virtual int Connect(const SocketAddress& address);
	virtual int Send(const char* buf, size_t size);
	virtual int SendTo(const char* buf, size_t size, const SocketAddress& address);
	virtual void Close();

	int StartRead();

	void ConnectionCallback(RequestBase* req);
	void WriteCallback(WriteRequest* req);
	void ReadCallback(ReadRequest* req);
	void CloseCallback(int status);

private:
	int InitSockfd();
	int SetSockfd(sockfd_t socket);
	void QueueReadRequest();

private:
	EventLoopAdapter* m_eventLoop;
	sockfd_t m_socket;
	unsigned int m_flags;
	SocketAddress m_remoteAddress;

	ReadRequest m_readRequest;
	char m_readBuffer[65536];
}; 

class TCPServerSocket : public ServerSocket
{
public:
	TCPServerSocket(EventLoopAdapter* eventLoop);
	~TCPServerSocket();

	virtual int Bind(const SocketAddress& bindAddress);
	virtual int Listen();
	virtual void Close();

	void ListenCallback(AcceptRequest* accpetRequest);
	void CloseCallback(ServerSocket* serverSock);

private:
	int InitSockfd();
	int SetSockfd(sockfd_t socket);
	void QueueTCPAccept(AcceptRequest* accpetRequest);

private:
	EventLoopAdapter* m_eventLoop;
	sockfd_t m_socket;
	unsigned int m_flags;
	LPFN_ACCEPTEX m_accpetExFunc;
	LPFN_GETACCEPTEXSOCKADDRS  m_getacceptexsockaddrs;

	AcceptRequest* m_accpetRequest;
};

#endif