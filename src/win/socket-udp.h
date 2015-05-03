#ifndef _BASE_SOCKET_UDP_H_
#define _BASE_SOCKET_UDP_H_

#include "socket-inl.h"
#include "eventloopadapter.h"
#include "windowheaders.h"

class UDPSocket : public Socket
{
public:
	UDPSocket(EventLoopAdapter* eventLoop);
	~UDPSocket();

	virtual int Bind(const SocketAddress& bindAddress);
	virtual int Connect(const SocketAddress& address);
	virtual int Send(const char* buf, uint32_t size);
	virtual int SendTo(const char* buf, uint32_t size, const SocketAddress& address);
	virtual void Close();

	void WriteCallback(WriteRequest* req);
	void ReadCallback(ReadRequest* req);

private:
	int InitSockfd();
	int SetSockfd(sockfd_t socket);
	int StartRead();
	void StopRead();
	void QueueReadRequest();

private:
	EventLoopAdapter* m_eventLoop;
	sockfd_t m_socket;
	unsigned int m_flags;

	struct sockaddr_storage m_recvFromAddr;
	//struct sockaddr_in m_recvFromAddr;
	int m_recvfromLen;

	ReadRequest m_readRequest;
	char m_readBuffer[65536];
};

#endif;