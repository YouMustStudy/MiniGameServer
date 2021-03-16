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
	ST_NOLOGIN,		//�α��� ���� ���� ����
	ST_IDLE,		//���ȭ�� ����
	ST_QUEUE,		//��ġ ��⿭�� ������ ����
	ST_PLAY,		//���� �÷�����
	ST_DISCONN		//���� �������
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

	//Recv�� ������ ������� ����, ���� recv�� recvOver������ �Ͼ!!
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