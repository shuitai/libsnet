#include "socketaddress.h"
#include "assertions.h"
#include <sstream>

SocketAddress::SocketAddress() 
	: m_ip(0)
	, m_port(0)
{

}

SocketAddress::SocketAddress(uint16_t port) 
	: m_ip(0)
	, m_port(port)
{
}

SocketAddress::SocketAddress(const char *ip, uint16_t port)
	: m_ip(IPAddress(ntohl(inet_addr(ip))))
	, m_port(port)
{
}

SocketAddress::SocketAddress(uint32_t ip, uint16_t port) 
	: m_ip(ip)
	, m_port(port)
{
}

SocketAddress::SocketAddress(const IPAddress& ip, uint16_t port)
	: m_ip(ip)
	, m_port(port)
{
}

SocketAddress::SocketAddress(const SocketAddress &sockAddress)
{
	this->operator=(sockAddress);
}

SocketAddress& SocketAddress::operator=(const SocketAddress& addr)
{
	m_ip = addr.m_ip;
	m_port = addr.m_port;

	return *this;
}

void SocketAddress::SetIPAddress(int ip)
{
	m_ip = IPAddress(ip);
}

void SocketAddress::SetIPAddress(const char* ip)
{
	m_ip = IPAddress(ntohl(inet_addr(ip)));
}

const IPAddress& SocketAddress::GetIPAddress() const
{
	return m_ip;
}

std::string SocketAddress::GetIPString() const
{
	return m_ip.ToString();
}

void SocketAddress::SetPort(uint16_t port)
{
	DEBUG_ASSERT(0 < port && port < 65535);
	m_port = port;
}

uint16_t SocketAddress::GetPort() const
{
	return m_port;
}

void SocketAddress::ToSockAddr(sockaddr_in* saddr) const
{
	if(saddr == NULL)
	{
		return;
	}

	saddr->sin_family = AF_INET;
	saddr->sin_port = htons(m_port);

	if(IpIsAny(m_ip))
	{
		saddr->sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		saddr->sin_addr = m_ip.IPV4Address();
	}
}

void SocketAddress::FromSockAddr(const sockaddr_in& saddr)
{
	m_ip = IPAddress(ntohl(saddr.sin_addr.s_addr));
	m_port = ntohs(saddr.sin_port);
}

static size_t ToSockAddrStorageHelper(sockaddr_storage* addr, IPAddress ip, uint16_t port, int scope_id) 
{
	memset(addr, 0, sizeof(sockaddr_storage));
	addr->ss_family = ip.Family();
	if (addr->ss_family == AF_INET6) 
	{
		sockaddr_in6* saddr = reinterpret_cast<sockaddr_in6*>(addr);
		saddr->sin6_addr = ip.IPV6Address();
		saddr->sin6_port = htons(port);
		saddr->sin6_scope_id = scope_id;
		return sizeof(sockaddr_in6);
	} 
	else if (addr->ss_family == AF_INET) 
	{
		sockaddr_in* saddr = reinterpret_cast<sockaddr_in*>(addr);
		saddr->sin_addr = ip.IPV4Address();
		saddr->sin_port = htons(port);
		return sizeof(sockaddr_in);
	}
	return 0;
}

size_t SocketAddress::ToSockAddrStorage(sockaddr_storage* saddr) const
{
	return ToSockAddrStorageHelper(saddr, m_ip, m_port, 0);
}

bool SocketAddress::operator ==(const SocketAddress& addr) const 
{
	return (m_ip == addr.m_ip && m_port == addr.m_port);
}

bool SocketAddressFromSockAddrStorage(const sockaddr_storage& saddrStorage, SocketAddress* out)
{
	if (!out) 
	{
		return false;
	}

	if (saddrStorage.ss_family == AF_INET) 
	{
		const sockaddr_in* saddr = reinterpret_cast<const sockaddr_in*>(&saddrStorage);
		*out = SocketAddress(IPAddress(saddr->sin_addr), ntohs(saddr->sin_port));
		return true;
	} 
	else if (saddrStorage.ss_family == AF_INET6) {
		const sockaddr_in6* saddr = reinterpret_cast<const sockaddr_in6*>(&saddrStorage);
		*out = SocketAddress(IPAddress(saddr->sin6_addr), ntohs(saddr->sin6_port));
		//out->SetScopeID(saddr->sin6_scope_id);
		return true;
	}
	return false;
}

