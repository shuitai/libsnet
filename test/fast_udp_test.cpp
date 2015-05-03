#include <stdio.h>
#include "sigslot.h"
#include "socket.h"
#include "assertions.h"
#include "eventloop.h"

#include <list>

class TCPClient;

void TestSend(TCPClient &channel, int count);

class TCPClient : public sigslot::has_slots<>
{
public:
	TCPClient(Socket* sock)
		: m_socket(sock)
	{
		m_socket->SignalConnectEvent.connect(this, &TCPClient::OnConnect);
		m_socket->SignalWriteEvent.connect(this, &TCPClient::OnWrite);
		m_socket->SignalReadEvent.connect(this, &TCPClient::OnRead);
		m_socket->SignalErrorEvent.connect(this, &TCPClient::OnError);
	}

	TCPClient()
	{
		m_socket = Socket::CreateSocket(NET_SOCK_STREAM);
		m_socket->SignalConnectEvent.connect(this, &TCPClient::OnConnect);
		m_socket->SignalWriteEvent.connect(this, &TCPClient::OnWrite);
		m_socket->SignalReadEvent.connect(this, &TCPClient::OnRead);
		m_socket->SignalErrorEvent.connect(this, &TCPClient::OnError);
	}
	
	~TCPClient()
	{
		delete m_socket;
	}

	int Connect(char *ipAddr, int port)
	{
		SocketAddress socketAddress;
		socketAddress.SetIPAddress(ipAddr);
		socketAddress.SetPort(port);
		return m_socket->Connect(socketAddress);
	}

	int Send(const char *pBuffer, int nLen)
	{
		return m_socket->Send(pBuffer, nLen);
	}

	void OnConnect(Socket* socket, int errorCode)
	{
		printf("[OnConnect]: %d\n", errorCode);
		//TestSend(*this, 1);
	}

	void OnWrite(Socket* socket, int status)
	{
		//printf("[OnWrite]: %d\n", status);
		//TestSend(*this, 1);

		/*sentBytes += sizeof(message);
		expectedSentBytes += sizeof(message);*/
	}

	void OnRead(Socket* socket, const char* buffer, uint32_t len, const SocketAddress& socketAdress)
	{
		//printf("[OnRead]: %d\n", len);
	}

	void OnError(Socket* socket, int error)
	{
		printf("[OnError]: %d\n", error);
	}

private:
	Socket* m_socket;
};

class TCPServer : public sigslot::has_slots<>
{
public:
	TCPServer(int nPort)
	{
		m_serverSocket = ServerSocket::CreateServerSocket(NET_SOCK_STREAM);
		m_serverSocket->SignalAcceptEvent.connect(this, &TCPServer::OnAcceptEvent);

		SocketAddress socketAddress(nPort);
		int ret = m_serverSocket->Bind(socketAddress);
		DEBUG_ASSERT(ret == 0);

		ret = m_serverSocket->Listen();
		DEBUG_ASSERT(ret == 0);
	}

	~TCPServer()
	{
		delete m_serverSocket;

		std::list<TCPClient*>::iterator iter = m_clientList.begin();
		while(iter != m_clientList.end())
		{
			delete *iter;
			++iter;
		}

		m_clientList.clear();
	}

	void OnAcceptEvent(ServerSocket* serverSock, Socket* socket)
	{
		printf("OnAcceptEvent!\n");
		m_clientList.push_back(new TCPClient(socket));
	}

private:
	ServerSocket* m_serverSocket;
	std::list<TCPClient*> m_clientList;
};

class UDPPeer : public sigslot::has_slots<>
{
public:
	UDPPeer()
	{
		m_socket = Socket::CreateSocket(NET_SOCK_DGRAM);
		m_socket->SignalWriteEvent.connect(this, &UDPPeer::OnWrite);
		m_socket->SignalReadEvent.connect(this, &UDPPeer::OnRead);
		m_socket->SignalErrorEvent.connect(this, &UDPPeer::OnError);
	}
	
	~UDPPeer()
	{
		delete m_socket;
	}

	int Bind(int port)
	{
		SocketAddress socketAddress(port);
		return m_socket->Bind(socketAddress);
	}

	int SendTo(const char *pBuffer, int nLen, const char* ipaddress)
	{
		SocketAddress socketAddress;
		socketAddress.SetIPAddress(ipaddress);
		socketAddress.SetPort(5555);
		return m_socket->SendTo(pBuffer, nLen, socketAddress);
	}

	void OnWrite(Socket* socket, int status)
	{
		//printf("[OnWrite]: %d\n", status);
		//TestSend(*this, 1);

		/*sentBytes += sizeof(message);
		expectedSentBytes += sizeof(message);*/
	}

	void OnRead(Socket* socket, const char* buf, uint32_t len, const SocketAddress& socketAddress)
	{
		printf("[OnRead]: %d, id: %s\n", len, (char*)buf);
	}

	void OnError(Socket* socket, int error)
	{
		printf("[OnError]: %d\n", error);
	}

private:
	Socket* m_socket;
};

#if 1
void TestTCPSend(TCPClient &channel, int count)
{
	int sentBytes = 0;
	int expectedSentBytes = 0;
	int loops = count;
	double rate = 0.0;
	char message[8192];

	//uint64_t begin = GetSystemNanoTime();
	int total = count;
	while(count > 0)
	{
		sprintf(message, "%d", total - count);
		int ret = channel.Send(message, sizeof(message));
		if (ret != 0)
		{
			printf("[TestSend]send error!\n");
			break;
		}
		
		count--;
	}

	/*uint64_t elapse = GetSystemNanoTime() - begin;

	printf("sent bytes: %d\n", sentBytes);
	printf("expected sent bytes: %d\n", expectedSentBytes);
	printf("loops: %d, cost time %lld [us]\n", loops, elapse / 1e3);
	printf("size : %f Mb \n ", (loops * (sizeof(message)) / 1024.0 / 1024.0));
	printf("time : %f secs \n ", (elapse / 1e9));
	rate = (double)(loops * (sizeof(message)) / 1024.0 / 1024.0) / (elapse / 1e9);
	printf("rate : %f M/s\n", rate);*/

}
#endif

void TestUDPSend(UDPPeer &channel, const char* ipaddress, int count)
{
	int sentBytes = 0;
	int expectedSentBytes = 0;
	int loops = count;
	double rate = 0.0;
	char message[1024];

	memset(message, 0, sizeof(message));

	//uint64_t begin = GetSystemNanoTime();

	int total = count;
	while(count > 0)
	{
		sprintf(message, "%d", total - count);
		int ret = channel.SendTo(message, sizeof(message), ipaddress);
		if (ret != 0)
		{
			printf("[TestUDPSend]send error: %d\n", ret);
			break;
		}

		//::Sleep(10);
		
		count--;
	}

	/*uint64_t elapse = GetSystemNanoTime() - begin;

	printf("sent bytes: %d\n", sentBytes);
	printf("expected sent bytes: %d\n", expectedSentBytes);
	printf("loops: %d, cost time %lld [us]\n", loops, elapse / 1e3);
	printf("size : %f Mb \n ", (loops * (sizeof(message)) / 1024.0 / 1024.0));
	printf("time : %f secs \n ", (elapse / 1e9));
	rate = (double)(loops * (sizeof(message)) / 1024.0 / 1024.0) / (elapse / 1e9);
	printf("rate : %f M/s\n", rate);*/

}

void TCPClientMode()
{
	EventLoop* loop = EventLoop::DefaultEventLoop();
	loop->Start();

	printf("===========Client Mode===========\n");
	printf("c: connect\n");
	printf("s: send\n");
	printf("=================================\n");
	char ipAddr[20];
	char c = 0;
	int count = 0;
	TCPClient clientChannel;

	int ret = -1;

	bool bRunning = true;
	while(bRunning)
	{
		c = getchar();
		switch(c)
		{
		case 'c':
			printf("please input remote ip >");
#if OS(WINDOWS)
			scanf_s("%s", ipAddr, 20);
#else
			scanf("%s", ipAddr);
#endif
			ret = clientChannel.Connect(ipAddr, 5555);
			if (ret != 0)
			{
				printf("connect ret: %d\n", ret);
			}
			break;
		case 's':
			printf("please input packet counts >");
#if OS(WINDOWS)
			scanf_s("%d", &count, 10);
#else
			scanf("%d", &count);
#endif
			TestTCPSend(clientChannel, count);
			break;
		case 'q':
			bRunning = false;
			break;
		default:
			break;
		}
	}
}


void TCPServerMode()
{
	EventLoop* loop = EventLoop::DefaultEventLoop();
	loop->Start();

	printf("===========Server Mode===========\n");
	printf("q: quit\n");
	printf("=================================\n");
	TCPServer serverChannel(5555);

	char c;
	bool bRunning = true;
	while(bRunning)
	{
		c = getchar();
		switch(c)
		{
		case 'q':
			bRunning = false;
			break;
		default:
			break;
		}
	}
}

void UDPPeerMode()
{
	EventLoop* loop = EventLoop::DefaultEventLoop();
	loop->Start();

	printf("===========UDP Peer Mode===========\n");
	printf("b: bind\n");
	printf("s: send\n");
	printf("q: quit\n");
	printf("=================================\n");
	UDPPeer udppeer;

	char ipAddr[20];
	int ret = -1;
	int count = 0;

	
	char c;
	bool bRunning = true;
	while(bRunning)
	{
		c = getchar();
		switch(c)
		{
		case 'q':
			bRunning = false;
			break;
		case 'b':
			udppeer.Bind(5555);
			break;
		case 's':
			udppeer.Bind(0);
			printf("please input remote ip >");
#if OS(WINDOWS)
			scanf_s("%s", ipAddr, 20);
#else
			scanf("%s", ipAddr);
#endif

			printf("please input packet counts >");
#if OS(WINDOWS)
			scanf_s("%d", &count, 10);
#else
			scanf("%d", &count);
#endif
			TestUDPSend(udppeer, ipAddr, count);
		default:
			break;
		}
	}
}

int main()
{
	printf("select mode--- 1: TCPClient, 2: TCPServer, 3: UDPPeer >>>>");
	char c = getchar();
	switch(c)
	{
	case '1':
		TCPClientMode();
		break;
	case '2':
		TCPServerMode();
		break;
	case '3':
		UDPPeerMode();
		break;
	case 'q':
		break;
	}
	return 0;
}