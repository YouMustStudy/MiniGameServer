#pragma once
#include <WS2tcpip.h>

#include "OverEx.h"
#include "PacketVector.h"
#include "..\protocol.h"


enum STATEMENT 
{ 
	ST_NOLOGIN, 
	ST_PLAY 
};
/**
@brief Client class for battle server.
@author Gurnwoo Kim
*/
class Client
{
	static int BUFFER_SIZE;

public:
	OverEx recvOver{}; ///< OVERRAPPED Wrapping class for IOCP.
	SOCKET socket{ INVALID_SOCKET }; ///< socket for client.

	int state{ ST_NOLOGIN };

	int uid{ 0 };								 ///< uid
	PacketVector savedPacket{ BUFFER_SIZE };	 ///< packet buffer.
	size_t savedSize{ 0 };						 ///< packet buffer data.
	size_t needSize{ sizeof(DEFAULT_PACKET) };	 ///< packet buffer data.

	Client() {};
	~Client() {};

	/**
	@brief set recv on iocp env.
	*/
	void SetRecv();
};