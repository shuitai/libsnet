#ifndef _BASE_SOCKETADDRESS_H_
#define _BASE_SOCKETADDRESS_H_

#include "ipaddress.h"

#undef SetPort

class SocketAddress
{
public:
	SocketAddress();
	SocketAddress(uint16_t port);
	SocketAddress(const char *ip, uint16_t port);
	SocketAddress(uint32_t ip, uint16_t port);
	SocketAddress(const IPAddress& ip, uint16_t port);
	SocketAddress(const SocketAddress& sockAddress);
	SocketAddress& operator=(const SocketAddress& addr);

	void SetIPAddress(int ip);
	void SetIPAddress(const char* ip);
	const IPAddress& GetIPAddress() const;
	std::string GetIPString() const;

	void SetPort(uint16_t port);
	uint16_t GetPort() const;

	void ToSockAddr(sockaddr_in* saddr) const;
	void FromSockAddr(const sockaddr_in& saddr);

	size_t ToSockAddrStorage(sockaddr_storage* saddr) const;

	bool operator==(const SocketAddress& addr) const;

	inline bool operator!=(const SocketAddress& addr) const 
	{
		return !this->operator==(addr);
	}

private:
	IPAddress m_ip;
	uint16_t m_port;
};

bool SocketAddressFromSockAddrStorage(const sockaddr_storage& saddr, SocketAddress* out);

#endif