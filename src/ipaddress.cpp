#include "ipaddress.h"
#include "byteorder.h"
#include "socket-inl.h"

IPAddress::IPAddress() 
	: m_family(AF_UNSPEC) 
{
	memset(&m_inAddr, 0, sizeof(m_inAddr));
}

IPAddress::IPAddress(const in_addr &ip4) 
	: m_family(AF_INET) 
{
	memset(&m_inAddr, 0, sizeof(m_inAddr));
	m_inAddr.ip4 = ip4;
}

IPAddress::IPAddress(const in6_addr &ip6) 
	: m_family(AF_INET6) 
{
	m_inAddr.ip6 = ip6;
}

IPAddress::IPAddress(uint32_t ip_in_host_byte_order) 
	: m_family(AF_INET) 
{
	memset(&m_inAddr, 0, sizeof(m_inAddr));
	m_inAddr.ip4.s_addr = htonl(ip_in_host_byte_order);
}

IPAddress::IPAddress(const IPAddress &other) 
	: m_family(other.m_family) 
{
	::memcpy(&m_inAddr, &other.m_inAddr, sizeof(m_inAddr));
}

IPAddress::~IPAddress() 
{}

const IPAddress& IPAddress::operator=(const IPAddress &other)
{
	m_family = other.m_family;
    ::memcpy(&m_inAddr, &other.m_inAddr, sizeof(m_inAddr));
    return *this;
}

bool IPAddress::operator==(const IPAddress &other) const
{
	if (m_family != other.m_family) 
	{
		return false;
	}
	if (m_family == AF_INET) 
	{
		return memcmp(&m_inAddr.ip4, &other.m_inAddr.ip4, sizeof(m_inAddr.ip4)) == 0;
	}
	if (m_family == AF_INET6) 
	{
		return memcmp(&m_inAddr.ip6, &other.m_inAddr.ip6, sizeof(m_inAddr.ip6)) == 0;
	}
	return m_family == AF_UNSPEC;
}

bool IPAddress::operator!=(const IPAddress &other) const
{
	return !((*this) == other);
}

bool IPAddress::operator <(const IPAddress &other) const
{
	if (m_family != other.m_family) 
	{
		if (m_family == AF_UNSPEC) 
		{
			return true;
		}
		if (m_family == AF_INET && other.m_family == AF_INET6)
		{
			return true;
		}
		return false;
	}
	switch (m_family) 
	{
	case AF_INET: 
		return ntohl(m_inAddr.ip4.s_addr) < ntohl(other.m_inAddr.ip4.s_addr);
	case AF_INET6: 
		return memcmp(&m_inAddr.ip6.s6_addr, &other.m_inAddr.ip6.s6_addr, 16) < 0;
	}
	return false;
}

bool IPAddress::operator >(const IPAddress &other) const
{
	return !((*this) < other);
}

std::ostream& operator<<(std::ostream& os, const IPAddress& addr)
{
	os << addr.ToString();
	return os;
}

in_addr IPAddress::IPV4Address() const
{
	return m_inAddr.ip4;
}

in6_addr IPAddress::IPV6Address() const
{
	return m_inAddr.ip6;
}

bool IPAddress::IsLoopback() const
{
	if (m_family == AF_INET)
	{
		return (ntohl(m_inAddr.ip4.s_addr) & 0xFF000000) == 0x7F000000;
	}
	else if (m_family == AF_INET6)
	{
		const uint16_t* words = reinterpret_cast<const uint16_t*>(&m_inAddr.ip6);
		return words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && 
		words[4] == 0 && words[5] == 0 && words[6] == 0 && ByteOrder::FromNetwork(words[7]) == 0x0001;
	}
	return false;
}

bool IPAddress::isMulticast() const
{
	if (m_family == AF_INET)
	{
		return (ntohl(m_inAddr.ip4.s_addr) & 0xF0000000) == 0xE0000000;
	}
	else if (m_family == AF_INET6)
	{
		const uint16_t* words = reinterpret_cast<const uint16_t*>(&m_inAddr.ip6);
		return (ByteOrder::FromNetwork(words[0]) & 0xFFE0) == 0xFF00;
	}
	return false;
}

size_t IPAddress::Size() const
{
	switch(m_family)
	{
	case AF_INET:
		return sizeof(in_addr);
	case AF_INET6:
		return sizeof(in6_addr);
	}

	return 0;
}

std::string IPAddress::ToString() const
{
	if (m_family != AF_INET && m_family != AF_INET6) 
	{
		return std::string();
	}

	char buf[INET6_ADDRSTRLEN] = {0};
	const void* src = &m_inAddr.ip4;
	if (m_family == AF_INET6) 
	{
		src = &m_inAddr.ip6;
	}

	if (!inet_ntop_t(m_family, (void*)src, buf, sizeof(buf))) 
	{
		return std::string();
	}
	return std::string(buf);
}

bool IpIsAny(const IPAddress& ip)
{
	switch (ip.Family()) 
	{
	case AF_INET:
		return ip == IPAddress(INADDR_ANY);
	case AF_INET6:
		return ip == IPAddress(in6addr_any);
	case AF_UNSPEC:
		return false;
	}
	return false;
}