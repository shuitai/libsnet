#include "socket-tcp.h"
#include "assertions.h"

static BOOL GetWinsockExfunction(SOCKET socket, GUID guid, void **target);

TCPSocket::TCPSocket(EventLoopAdapter* eventLoop)
	: m_eventLoop(eventLoop)
	, m_socket(INVALID_SOCKET)
	, m_flags(0)
{
	m_readRequest.type = REQ_READ;
	m_readRequest.data = this;
}

TCPSocket::TCPSocket(EventLoopAdapter* eventLoop, sockfd_t incomeSock, const SocketAddress& address)
	: m_eventLoop(eventLoop)
	, m_socket(incomeSock)
	, m_flags(0)
	, m_remoteAddress(address)
{
	if (m_socket != INVALID_SOCKET)
	{
		m_flags |= HANDLE_READABLE | HANDLE_WRITABLE;
		DEBUG_ASSERT(SetSockfd(m_socket) == 0);
	}

	m_readRequest.type = REQ_READ;
	m_readRequest.data = this;
}

TCPSocket::~TCPSocket()
{
	Close();
}

int TCPSocket::Bind(const SocketAddress& address)
{
	DWORD err = 0;
	int ret = InitSockfd();
	if (ret != 0)
	{
		return ret;
	}

	sockaddr_in sockAddress;
	address.ToSockAddr(&sockAddress);
	ret = bind(m_socket, (struct sockaddr*)&sockAddress, sizeof(sockaddr_in));

	if (ret == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		return ret;
	}

	m_flags |= HANDLE_BOUND;
	return ret;
}

int TCPSocket::Connect(const SocketAddress& address)
{
	BOOL ret;
	sockaddr_in sockAddress;
	DWORD bytes;
	GUID guid = WSAID_CONNECTEX;
	LPFN_CONNECTEX connectex;

	if (!(m_flags & HANDLE_BOUND))
	{
		int retVal = Bind(SocketAddress());
		if (retVal != 0)
		{
			return retVal;
		}
	}

	ret = GetWinsockExfunction(m_socket, guid, (void**)&connectex);
	if (!ret)
	{
		return WSAEAFNOSUPPORT;
	}

	RequestBase* connReq = (RequestBase*) malloc(sizeof(RequestBase));
	connReq->type = REQ_CONNECT;
	connReq->data = this;
	memset(&(connReq->overlapped), 0, sizeof(connReq->overlapped));

	address.ToSockAddr(&sockAddress);
	ret = connectex(m_socket, (sockaddr*)&sockAddress,
                                   sizeof(sockaddr_in),
                                   NULL,
                                   0,
                                   &bytes,
                                   &(connReq->overlapped));
	if (SUCCEEDED_WITH_IOCP(ret))
	{
		return 0;
	}
	return WSAGetLastError();
}

int TCPSocket::Send(const char* buf, uint32_t size)
{
	int result = 0;
	DWORD bytes = 0;
	if (!(m_flags & HANDLE_WRITABLE)) 
	{
		return -1;
	}

	WriteRequest* writeReq = (WriteRequest*)malloc(sizeof(WriteRequest));
	writeReq->type = REQ_WRITE;
	writeReq->data = this;
	memset(&(writeReq->overlapped), 0, sizeof(writeReq->overlapped));

	WSABUF wsabuf;
	wsabuf.buf = (char*)buf;
	wsabuf.len = size;
	result = WSASend(m_socket, (WSABUF*)&wsabuf, 1, &bytes, 0, &writeReq->overlapped, NULL);

	if (!SUCCEEDED_WITH_IOCP(result == 0))
	{
		return WSAGetLastError();
	}

	return 0;
}

int TCPSocket::SendTo(const char* buf, uint32_t size, const SocketAddress& address)
{
	return -1;
}

void TCPSocket::Close()
{
	if (m_flags & HANDLE_READ_PENDING)
	{
		shutdown(m_socket, SD_SEND);
	}

	if (m_flags & HANDLE_READING) 
	{
		m_flags &= ~HANDLE_READING;
	}

	m_flags &= ~(HANDLE_READABLE | HANDLE_WRITABLE);

	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

int TCPSocket::StartRead()
{
	if (m_flags & HANDLE_READING)
	{
		return 0;
	}

	if (!(m_flags & HANDLE_READABLE)) 
	{
		return -1;
	}

	m_flags |= HANDLE_READING;

	if (!(m_flags & HANDLE_READ_PENDING))
	{
		QueueReadRequest();
	}

	return 0;
}

void TCPSocket::ConnectionCallback(RequestBase* req)
{
	int err = 0;
	if (REQ_SUCCESS(req)) 
	{
		if (setsockopt(m_socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0) == 0)
		{
			m_flags |= (HANDLE_READABLE | HANDLE_WRITABLE);
		} 
		else 
		{
			err = WSAGetLastError();
		}
	} 
	else 
	{
		err = -1;
	}

	free(req);
	StartRead();

	SignalConnectEvent(this, err);
}

void TCPSocket::WriteCallback(WriteRequest* req)
{
	int err = GET_REQ_STATUS(req);
	free(req);
	SignalWriteEvent(this, err);
}

void TCPSocket::ReadCallback(ReadRequest* req)
{
	DWORD bytes = 0, flags = 0, err = 0;
	m_flags &= ~HANDLE_READ_PENDING;

	if (!REQ_SUCCESS(req))
	{
		if (m_flags & HANDLE_READING)
		{
			m_flags &= ~HANDLE_READING;
		}
		
		SignalErrorEvent(this, GET_REQ_STATUS(req));
	}
	else
	{
		if (req->overlapped.InternalHigh > 0)
		{
			SignalReadEvent(this, m_readBuffer, req->overlapped.InternalHigh, m_remoteAddress);

			if (req->overlapped.InternalHigh >= sizeof(m_readBuffer))
			{
				while (m_flags & HANDLE_READING)
				{
					WSABUF wsabuf;
					wsabuf.buf = m_readBuffer;
					wsabuf.len = sizeof(m_readBuffer);

					flags = 0; // set the flag 0, or else recv function will return 10045.
					if (WSARecv(m_socket, (WSABUF*)&wsabuf, 1, &bytes, &flags, NULL, NULL) != SOCKET_ERROR) 
					{
						if (bytes > 0) 
						{
							/* Successful read */
							SignalReadEvent(this, m_readBuffer, bytes, m_remoteAddress);
							/* Read again only if bytes == buf.len */
							if (bytes < sizeof(m_readBuffer)) 
							{
								break;
							}
						} 
						else 
						{
							/* Connection closed */
							m_flags &= ~(HANDLE_READING | HANDLE_READABLE);
							SignalErrorEvent(this, -1);
							break;
						}
					}
					else 
					{
						err = WSAGetLastError();
						if (err == WSAEWOULDBLOCK) 
						{
							/* Read buffer was completely empty, report a 0-byte read. */
							//SignalReadEvent(this, (uint8_t*)m_readBuffer, 0);
						} 
						else 
						{
							/* Ouch! serious error. */
							m_flags &= ~HANDLE_READING;
							if (err == WSAECONNABORTED) 
							{
								/* Turn WSAECONNABORTED into UV_ECONNRESET to be consistent with */
								/* Unix. */
								err = WSAECONNRESET;
							}

							SignalErrorEvent(this, err);
						}	
						break;
					}
				}
			}
		}
		else 
		{
			/* Connection closed */
			if (m_flags & HANDLE_READING)
			{
				m_flags &= ~HANDLE_READING;
			}
			m_flags &= ~HANDLE_READABLE;
			SignalErrorEvent(this, 0);
		}
	}
	if ((m_flags & HANDLE_READING) && !(m_flags & HANDLE_READ_PENDING)) 
	{
		QueueReadRequest();
	}
}

void TCPSocket::CloseCallback(int status)
{}

int TCPSocket::InitSockfd()
{
	DWORD err = 0;
	if (m_socket == INVALID_SOCKET)
	{
		sockfd_t newSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (newSocket == INVALID_SOCKET) 
		{
			return WSAGetLastError();
		}

		if (!SetHandleInformation((HANDLE)newSocket, HANDLE_FLAG_INHERIT, 0)) 
		{
			err = GetLastError();
			closesocket(newSocket);
			return err;
		}

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

int TCPSocket::SetSockfd(sockfd_t socket)
{
	DWORD yes = 1;
	/* Set the socket to nonblocking mode */
	if (ioctlsocket(socket, FIONBIO, &yes) == SOCKET_ERROR) 
	{
		return WSAGetLastError();
	}

	/* Associate it with the I/O completion port. */
	/* Use uv_handle_t pointer as completion key. */
	if (CreateIoCompletionPort((HANDLE)socket, m_eventLoop->m_iocpPoll->m_iocp, 
			                   (ULONG_PTR)socket, 0) == NULL) 
	{
		return GetLastError();
	}

	return 0;
}

void TCPSocket::QueueReadRequest()
{
	ReadRequest* req = &m_readRequest;
	memset(&req->overlapped, 0, sizeof(req->overlapped));

	DWORD bytes = 0, flags = 0;
	WSABUF wsaBuffer;
	wsaBuffer.buf = m_readBuffer;
	wsaBuffer.len = sizeof(m_readBuffer);
	int result = WSARecv(m_socket, (WSABUF*)&wsaBuffer, 1, &bytes, &flags, &req->overlapped, NULL);
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

//TCPServerSocket
TCPServerSocket::TCPServerSocket(EventLoopAdapter* eventLoop)
	: m_eventLoop(eventLoop)
	, m_socket(INVALID_SOCKET)
	, m_accpetExFunc(NULL)
	, m_getacceptexsockaddrs(NULL)
	, m_accpetRequest(NULL)
	, m_flags(0)
{
}

TCPServerSocket::~TCPServerSocket()
{
	Close();
}

int TCPServerSocket::Bind(const SocketAddress& bindAddress)
{
	int result = InitSockfd();
	if (result != 0)
	{
		return result;
	}

	sockaddr_in sockAddress;
	bindAddress.ToSockAddr(&sockAddress);
	result = bind(m_socket, (struct sockaddr*)&sockAddress, sizeof(sockaddr_in));

	if (result == 0)
	{
		m_flags |= HANDLE_BOUND;
	}

	return result;
}

int TCPServerSocket::Listen()
{
	if (m_accpetExFunc == NULL)
	{
		GUID guid = WSAID_ACCEPTEX;
		BOOL ret = GetWinsockExfunction(m_socket, guid, (void**)&m_accpetExFunc);
		if (!ret)
		{
			m_accpetExFunc = NULL;
			return WSAEAFNOSUPPORT;
		}

		if (m_getacceptexsockaddrs == NULL)
		{
			GUID guid = WSAID_GETACCEPTEXSOCKADDRS;
			BOOL ret = GetWinsockExfunction(m_socket, guid, (void**)&m_getacceptexsockaddrs);
			if (!ret)
			{
				m_accpetExFunc = NULL;
				return WSAEAFNOSUPPORT;
			}
		}
	}

	if (listen(m_socket, 5) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	m_flags |= HANDLE_LISTENING;

	if (m_accpetRequest == NULL)
	{
		m_accpetRequest = (AcceptRequest*)malloc(sizeof(AcceptRequest));
		m_accpetRequest->type = REQ_ACCEPT;
		m_accpetRequest->acceptSocket = INVALID_SOCKET;
		m_accpetRequest->data = this;
		m_accpetRequest->waitHandle = INVALID_HANDLE_VALUE;
		m_accpetRequest->eventHandle = NULL;

		QueueTCPAccept(m_accpetRequest);
	}
	return 0;
}

void TCPServerSocket::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

void TCPServerSocket::ListenCallback(AcceptRequest* accpetRequest)
{
	if (REQ_SUCCESS(accpetRequest))
	{
		int ret = setsockopt(accpetRequest->acceptSocket, SOL_SOCKET, 
			                 SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_socket, sizeof(m_socket));
		if (ret == 0)
		{
			sockaddr_storage* localAddr = NULL;
			sockaddr_storage* remoteAddr = NULL;
			socklen_t         len;
			m_getacceptexsockaddrs(
				accpetRequest->acceptBuffer, 0,
				sizeof(sockaddr_storage)+16, sizeof(sockaddr_storage)+16,
				(struct sockaddr**)&localAddr, &len,
				(struct sockaddr**)&remoteAddr, &len);

			TCPSocket* newSock = NULL;
			
			SocketAddress remoteAddress;
			if (SocketAddressFromSockAddrStorage(*remoteAddr, &remoteAddress))
			{
				newSock = new TCPSocket(m_eventLoop, accpetRequest->acceptSocket, remoteAddress);
				newSock->StartRead();
				SignalAcceptEvent(this, newSock);
			}
			else
			{
				DEBUG_ASSERT(0);
			}
		}

		QueueTCPAccept(accpetRequest);
	}
}

void TCPServerSocket::CloseCallback(ServerSocket* serverSock)
{}

int TCPServerSocket::InitSockfd()
{
	DWORD err;
	if (m_socket == INVALID_SOCKET)
	{
		sockfd_t newSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (newSocket == INVALID_SOCKET) 
		{
			return WSAGetLastError();
		}

		if (!SetHandleInformation((HANDLE)newSocket, HANDLE_FLAG_INHERIT, 0)) 
		{
			err = GetLastError();
			closesocket(newSocket);
			return err;
		}

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

int TCPServerSocket::SetSockfd(sockfd_t socket)
{
	DWORD yes = 1;
	/* Set the socket to nonblocking mode */
	if (ioctlsocket(socket, FIONBIO, &yes) == SOCKET_ERROR) 
	{
		return WSAGetLastError();
	}

	/* Associate it with the I/O completion port. */
	/* Use uv_handle_t pointer as completion key. */
	if (CreateIoCompletionPort((HANDLE)socket, m_eventLoop->m_iocpPoll->m_iocp, 
			                   (ULONG_PTR)socket, 0) == NULL) 
	{
		return GetLastError();
	}

	return 0;
}

void TCPServerSocket::QueueTCPAccept(AcceptRequest* accpetRequest)
{
	BOOL success;
	DWORD bytes;
	SOCKET acceptSocket = socket(AF_INET, SOCK_STREAM, 0);

	/* Open a socket for the accepted connection. */
	if (acceptSocket == INVALID_SOCKET) 
	{
		SET_REQ_ERROR(accpetRequest, WSAGetLastError());
		m_eventLoop->PushToPendingQueue((RequestBase*)accpetRequest);
		return;
	}

	/* Make the socket non-inheritable */
	if (!SetHandleInformation((HANDLE) acceptSocket, HANDLE_FLAG_INHERIT, 0)) 
	{
		SET_REQ_ERROR(accpetRequest, GetLastError());
		m_eventLoop->PushToPendingQueue((RequestBase*)accpetRequest);
		closesocket(acceptSocket);
		return;
	}

	/* Prepare the overlapped structure. */
	memset(&(accpetRequest->overlapped), 0, sizeof(accpetRequest->overlapped));

	success = m_accpetExFunc(m_socket, acceptSocket,
							(void*)accpetRequest->acceptBuffer,
							0,
							sizeof(struct sockaddr_storage) + 16,
							sizeof(struct sockaddr_storage) + 16,
							&bytes,
							&accpetRequest->overlapped);

	if (SUCCEEDED_WITH_IOCP(success)) 
	{
		/* The req will be processed with IOCP. */
		accpetRequest->acceptSocket = acceptSocket;
	}
}

static BOOL GetWinsockExfunction(SOCKET socket, GUID guid, void **target) 
{
	int result;
	DWORD bytes;

	result = WSAIoctl(socket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guid,
				sizeof(guid),
				(void*)target,
				sizeof(*target),
				&bytes,
				NULL,
				NULL);

	if (result == SOCKET_ERROR) 
	{
		*target = NULL;
		return FALSE;
	}
	return TRUE;
}