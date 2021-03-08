#pragma once
#include <WS2tcpip.h>

/**
@brief Wrapping class for SOCKADDR structure.
@author Gurnwoo Kim
*/
class CSOCKADDR_IN
{
private:
	int size; ///< size of SOCKADDR_IN.
	SOCKADDR_IN addr; ///< SOCKADDR_IN structure.

public:
	CSOCKADDR_IN();

	/**
	@brief Constructor for unsigned long type addr. no need to convert NW ENDIAN.
	*/
	CSOCKADDR_IN(unsigned long addr, short port);
	/**
	@brief Constructor for char type addr.
	*/
	CSOCKADDR_IN(const char* addr, short port);
	/**
	@brief Constructor for char type addr.
	*/
	CSOCKADDR_IN(const wchar_t* addr, short port);

	/**
	@brief return pointer of size. remember it can be changed.
	@return pointer of int type size.
	*/
	int* Len();
	/**
	@brief return pointer of SOCKADDR_IN.
	@return pointer of SOCKADDR_IN to SOCKADDR type.
	*/
	SOCKADDR* GetSockAddr();
};