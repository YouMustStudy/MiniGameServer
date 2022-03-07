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
	ST_NOLOGIN,	//로그인 이전 세션 상태
	ST_IDLE,	//대기화면 상태
	ST_QUEUE,	//매치 대기열에 진입한 상태
	ST_PLAY,	//게임 플레이중
	ST_DISCONN	//연결 종료상태
};

class User
{
public:
	OverEx        recvOver{};                ///< OVERRAPPED Wrapping class for IOCP.
	SOCKET        socket{ INVALID_SOCKET };  ///< Socket for client.
	SOCKADDR_IN   addr{};                    ///< Address

	int           state{ ST_DISCONN };       ///< UserState
	UID           uid{ (UID)-1 };			 ///< uid
	std::string   id;                        ///< id
	unsigned char characterType{};           ///< Character Type
	class DMRoom* roomPtr{nullptr};          ///< Room pointer

	//Recv한 데이터 재저장용 버퍼, 실제 recv는 recvOver내에서 일어남!!
	PacketVector savedPacket{ BUFFER_SIZE }; ///< packet buffer.
	size_t needSize{ sizeof(PACKET_SIZE) };	 ///< minimum size of Packet.

	/**
	*@brief Default Constructor.
	*/
	User() {};

	/**
	*@brief Default Deconstructor.
	*/
	~User() {};

	/**
	*@brief set recv on iocp env.
	*/
	void SetRecv();

private:
	static constexpr size_t BUFFER_SIZE = 1024;
};