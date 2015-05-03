#include "socket-udp.h"
#include "assertions.h"
#include "error-inl.h"

UDPSocket::UDPSocket(EventLoopAdapter* eventLoop)
	: m_eventLoop(eventLoop)
	, m_socket(INVALID_SOCKET)
	, m_flags(0)
{
	m_readRequest.type = REQ_UDP_READ;
	m_readRequest.data = this;
}

UDPSocket::~UDPSocket()
{
	Close();
}

int UDPSocket::Bind(const SocketAddress& bindAddress)
{
	if (m_flags & HANDLE_BOUND)
	{
		return 0;
	}

	DWORD err = 0;
	int ret = InitSockfd();
	if (ret != 0)
	{
		return ret;
	}

	sockaddr_in sockAddress;
	bindAddress.ToSockAddr(&sockAddress);
	ret = bind(m_socket, (struct sockaddr*)&sockAddress, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		SignalErrorEvent(this, err);
		return ret;
	}

	m_flags |= HANDLE_BOUND;
	StartRead();
	return ret;
}

int UDPSocket::Connect(const SocketAddress& address)
{
	return -1;
}

int UDPSocket::Send(const char* buf, uint32_t size)
{
	return -1;
}

int UDPSocket::SendTo(const char* buf, uint32_t size, const SocketAddress& address)
{
	DWORD result = 0, bytes = 0;

	WriteRequest* writeReq = (WriteRequest*)malloc(sizeof(WriteRequest));
	writeReq->type = REQ_UDP_WRITE;
	writeReq->data = this;
	memset(&(writeReq->overlapped), 0, sizeof(writeReq->overlapped));

	WSABUF wsabuf;
	wsabuf.buf = (char*)buf;
	wsabuf.len = size;

	sockaddr_storage saddr;
    size_t len = address.ToSockAddrStorage(&saddr);

	result = WSASendTo(m_socket, (WSABUF*)&wsabuf, 1, &bytes, 0, (struct sockaddr*)&saddr, len, &writeReq->overlapped, NULL);
	if (!SUCCEEDED_WITH_IOCP(result == 0)) 
	{
		return WSAGetLastError();
	}

	return 0;
}

void UDPSocket::WriteCallback(WriteRequest* req)
{
	int err = 0;
    if (!REQ_SUCCESS(req)) 
	{
		err = GET_REQ_SOCK_ERROR(req);
    }

	free(req);
	SignalWriteEvent(this, err);
}

void UDPSocket::ReadCallback(ReadRequest* req)
{
	int partial = 0;
	SocketAddress remoteAddr;
	m_flags &= ~HANDLE_READ_PENDING;

	if (!REQ_SUCCESS(req))
	{
		DWORD err = GET_REQ_SOCK_ERROR(req);
		if (err == WSAEMSGSIZE) 
		{
			/* Not a real error, it just indicates that the received packet */
			/* was bigger than the receive buffer. */
		} 
		else if (err == WSAECONNRESET || err == WSAENETRESET) 
		{
			goto done;
		} 
		else {
			/* A real error occurred. Report the error to the user only if we're */
			/* currently reading. */
			if (m_flags & HANDLE_READING) 
			{
				StopRead();
				SignalErrorEvent(this, err);
			}
			goto done;
		}
	}

	partial = !REQ_SUCCESS(req);
	SocketAddressFromSockAddrStorage(m_recvFromAddr, &remoteAddr);
	SignalReadEvent(this, m_readBuffer, req->overlapped.InternalHigh, remoteAddr);

done:
	if ((m_flags & HANDLE_READING) && !(m_flags & HANDLE_READ_PENDING)) 
	{
		QueueReadRequest();
	}
}

void UDPSocket::Close()
{
	StopRead();
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
}

int UDPSocket::InitSockfd()
{
	DWORD err = 0;
	if (m_socket == INVALID_SOCKET)
	{
		sockfd_t newSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
		// newSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if (newSocket == INVALID_SOCKET) 
		{
			return WSAGetLastError();
		}

		/*if (!SetHandleInformation((HANDLE)newSocket, HANDLE_FLAG_INHERIT, 0)) 
		{
			err = GetLastError();
			closesocket(newSocket);
			return err;
		}*/

		err = SetSockfd(newSocket);
		if (err)
		{
			closesocket(newSocket);
			return err;
		}

		m_socket = newSocket;
	}

	return 0;
}

int UDPSocket::SetSockfd(sockfd_t socket)
{
	/* Set the socket to nonblocking mode */
	/*DWORD yes = 1;
	if (ioctlsocket(socket, FIONBIO, &yes) == SOCKET_ERROR) 
	{
		return WSAGetLastError();
	}*/

	/* Associate it with the I/O completion port. */
	/* Use uv_handle_t pointer as completion key. */
	if (CreateIoCompletionPort((HANDLE)socket, m_eventLoop->m_iocpPoll->m_iocp, 
			                   (ULONG_PTR)socket, 0) == NULL) 
	{
		return GetLastError();
	}

	return 0;
}

int UDPSocket::StartRead()
{
	if (m_flags & HANDLE_READING)
	{
		return WSAEALREADY;
	}
	
	m_flags |= HANDLE_READING;
	if (!(m_flags & HANDLE_READ_PENDING))
	{
		QueueReadRequest();
	}
	
	return 0;
}

void UDPSocket::StopRead()
{
	if (m_flags & HANDLE_READING) 
	{
		m_flags &= ~HANDLE_READING;
	}
}

void UDPSocket::QueueReadRequest()
{
	DEBUG_ASSERT(m_flags & HANDLE_READING);
	DEBUG_ASSERT(!(m_flags & HANDLE_READ_PENDING));

	ReadRequest* req = &m_readRequest;
	memset(&req->overlapped, 0, sizeof(req->overlapped));

	memset(&m_recvFromAddr, 0, sizeof(m_recvFromAddr));
	m_recvfromLen = sizeof(m_recvFromAddr);

	DWORD bytes = 0, flags = 0;
	WSABUF wsaBuffer;
	wsaBuffer.buf = m_readBuffer;
	wsaBuffer.len = sizeof(m_readBuffer);

	int result = WSARecvFrom(m_socket, (WSABUF*) &wsaBuffer, 1, &bytes, &flags, 
		(struct sockaddr*) &m_recvFromAddr, &m_recvfromLen, &req->overlapped, NULL);

	if (SUCCEEDED_WITH_IOCP(result == 0)) 
	{
		/* The req will be processed with IOCP. */
		m_flags |= HANDLE_READ_PENDING;
	}
	else 
	{
		/* Make this req pending reporting an error. */
		SET_REQ_ERROR(req, WSAGetLastError());
		m_eventLoop->PushToPendingQueue((RequestBase*)req);
	}
}