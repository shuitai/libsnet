#ifndef _BASE_IP_ADDRESS_H_
#define _BASE_IP_ADDRESS_H_

#include "common.h"
#include <string>

#if OS(WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h> 
#include <windows.h>
#include <process.h>
#include <iphlpapi.h>
#include <sys/timeb.h>
#else
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#endif

class IPAddress 
{
public:
	IPAddress();
	explicit IPAddress(const in_addr &ip4);
	explicit IPAddress(const in6_addr &ip6);
	explicit IPAddress(uint32_t ip_in_host_byte_order);
	IPAddress(const IPAddress &other);
	~IPAddress();

	const IPAddress& operator=(const IPAddress &other);

	bool operator==(const IPAddress &other) const;
	bool operator!=(const IPAddress &other) const;
	bool operator<(const IPAddress &other) const;
	bool operator>(const IPAddress &other) const;
	friend std::ostream& operator<<(std::ostream& os, const IPAddress& addr);

	int Family() const 
	{
		return m_family;
	}

	in_addr IPV4Address() const;
	in6_addr IPV6Address() const;

	bool IsLoopback() const;
	bool isMulticast() const;

	// Returns the number of bytes needed to store the raw address.
	size_t Size() const;

	// Wraps inet_ntop.
	std::string ToString() const;

private:
	int m_family;
	
	union 
	{
		in_addr ip4;
		in6_addr ip6;
	} m_inAddr;
};

bool IpIsAny(const IPAddress& ip);

#endif