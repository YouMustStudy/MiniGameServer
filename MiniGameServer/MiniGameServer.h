#pragma once
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <map>
#include <chrono>
#include <queue>
#include <iostream>

#include ".\Common\CSOCKADDR_IN.h"
#include ".\Common\Event.h"
#include ".\Common\OverEx.h"
#include ".\Common\Client.h"
#include ".\Rooms\DMRoom.h"
#include ".\Utills\Logger.h"
#include "protocol.h"

#pragma comment(lib, "ws2_32")

enum EVENT_TYPE;
enum CL_STATE;
constexpr auto RECV_BUF_SIZE = 512;

/**
@brief �̴ϰ��Ӽ���
@author Gurnwoo Kim
*/
class MiniGameServer
{
private:
	int UPDATE_INTERVAL{ 20 };		///< Interval of updateing time. Millisecond.
	int NUM_THREADS{ 6 };
	short SERVER_PORT{ 15600 };

	std::atomic<int> playerNum;		///< �� ���� �ο� ��
	HANDLE m_iocp;					///< �۾� ������� IOCP �ڵ�
	SOCKET m_listenSocket;			///< ���� ����

	DMRoom m_room;							///< ������ ����Ǵ� ����

	//Ÿ�̸�
	std::priority_queue<Event> timer_queue; ///< Ÿ�̸� ť
	std::mutex timer_lock;					///< Ÿ�̸� ť ��
	std::vector<std::thread> m_threads;		///< ������ ���

	/**
	@brief WSA �ʱ�ȭ.
	*/
	void InitWSA();

	/**
	@brief Ÿ�̸�, �۾� ������ �ʱ�ȭ.
	*/
	void InitThreads();

	/**
	@brief �� �ʱ�ȭ.
	*/
	void InitRooms();

	/**
	@brief �ɼǰ��� �о�´�. - ���� �̱���
	*/
	void LoadConfig();

	/**
	@brief Ÿ�̸� �̺�Ʈ�� �߰��Ѵ�.
	@param[in] ev �߰��� �̺�Ʈ
	*/
	void add_timer(Event& ev);

	/**
	@brief Ÿ�̸� �̺�Ʈ�� �߰��Ѵ�.
	@param[in] client �̺�Ʈ�� ���
	@param[in] et �̺�Ʈ ����
	@param[in] delay_time �̺�Ʈ ��������� ������
	*/
	void AddEvent(int client, EVENT_TYPE et, int delay_time);

	/**
	@brief ������ ��Ŷ�� �������Ѵ�.
	@param[in] client ������ ���
	@param[in] buffer ���� ����
	@param[in] recvLength ������ ũ��
	*/
	void ParsePacket(Client* client, void* buffer, size_t recvLength);

	/**
	@brief Ÿ�̸� ������ �Լ�
	*/
	void TimerThread();
	/**
	@brief �۾� ������ �Լ�
	*/
	void WorkerThread();

	/**
	@brief �÷��̾� ���� ó��
	*/
	void DisconnectPlayer(Client* client);

	/**
	@brief �������� ��Ŷ�� ������
	@param[in] client ���۹��� ����
	@param[in] buff ������ ������
	*/
	void send_packet(Client* client, void* buff);

	/**
	@brief Send default type packet to client.
	@param client target client.
	@param TYPE packet type.
	*/
	void send_packet_default(Client* client, int TYPE);

	/**
	@brief Process authorize packet received from clients.
	@param client sender.
	@param buffer packet data.
	*/
	void ProcessAuthoPacket(Client* client, void* buffer);

public:
	MiniGameServer();
	~MiniGameServer();

	/**
	@brief ������ ������.
	*/
	void Run();
};