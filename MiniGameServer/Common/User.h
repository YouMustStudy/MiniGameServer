#pragma once
#include <WS2tcpip.h>
#include <string>

#include "OverEx.h"
#include "PacketVector.h"
#include "..\protocol.h"

/**
@brief Client class for battle server.
@author Gurnwoo Kim
*/
enum UserState
{
	ST_NOLOGIN,		//로그인 이전 세션 상태
	ST_IDLE,		//대기화면 상태
	ST_QUEUE,		//매치 대기열에 진입한 상태
	ST_PLAY,		//게임 플레이중
	ST_DISCONN		//연결 종료상태
};

class User
{
public:
	OverEx recvOver{}; ///< OVERRAPPED Wrapping class for IOCP.
	SOCKET socket{ INVALID_SOCKET }; ///< socket for client.
	SOCKADDR_IN addr{};

	int state{ST_DISCONN};
	UID uid{ (UID)-1 };								 ///< uid
	std::string id;
	unsigned char characterType{};
	class DMRoom* roomPtr{nullptr};

	//Recv한 데이터 재저장용 버퍼, 실제 recv는 recvOver내에서 일어남!!
	PacketVector savedPacket{ BUFFER_SIZE }; ///< packet buffer.
	size_t needSize{ sizeof(PACKET_SIZE) };	 ///< packet buffer data.

	User() {};
	~User() {};

	/**
	@brief set recv on iocp env.
	*/
	void SetRecv();

private:
	static constexpr size_t BUFFER_SIZE = 1024;
};