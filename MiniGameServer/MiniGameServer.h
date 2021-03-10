#pragma once
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <queue>

#include ".\Common\CSOCKADDR_IN.h"
#include ".\Common\Event.h"
#include ".\Common\OverEx.h"
#include ".\Common\User.h"
#include ".\Rooms\DMRoom.h"
#include ".\Utills\Logger.h"
#include "protocol.h"

#pragma comment(lib, "ws2_32")

enum EVENT_TYPE { EV_ACCEPT, EV_DISCONN, EV_RECV, EV_SEND, EV_UPDATE };
constexpr auto RECV_BUF_SIZE = 1024;

/**
@brief �̴ϰ��Ӽ���
@author Gurnwoo Kim
*/
class MiniGameServer
{
private:
	size_t NUM_THREADS{ 10 };
	short SERVER_PORT{ 15600 };

	std::atomic<int> m_playerNum;		///< �� ���� �ο� ��
	HANDLE m_iocp;						///< �۾� ������� IOCP �ڵ�
	SOCKET m_listenSocket;				///< ���� ����

	//Ÿ�̸�
	std::priority_queue<Event> m_timerQueue;	///< Ÿ�̸� ť
	std::mutex m_timerLock;						///< Ÿ�̸� ť ��
	std::vector<std::thread> m_threads;			///< ������ ���

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
	void AddTimer(Event& ev);

	/**
	@brief ������ ��Ŷ�� �������Ѵ�.
	@param[in] client ������ ���
	@param[in] buffer ���� ����
	@param[in] recvLength ������ ũ��
	*/
	void ParsePacket(size_t idx, User* client, void* buffer, size_t recvLength);

	/**
	@brief ��Ŷ �ؼ� �� ������ �Ŵ������� �뺸�Ѵ�.
	@param[in] idx ������ ������ �ε���
	@param[in] buffer ��Ŷ�� ��� ����
	*/
	void ProcessPacket(User* user, size_t idx, void* buffer);

	/**
	@brief Ÿ�̸� ������ �Լ�
	*/
	void TimerThread();
	/**
	@brief �۾� ������ �Լ�
	*/
	void WorkerThread();

	void PostEvent(size_t key, int eventType);
	void PostEvent(size_t key, int eventType, void* args);

	MiniGameServer();
public:
	static MiniGameServer& Instance();
	~MiniGameServer();

	/**
	@brief �������� ��Ŷ '�ϳ�'�� ������
	@param[in] client ���۹��� ����
	@param[in] buff ������ ������
	*/
	void SendPacket(User* client, void* buff);

	/**
	@brief �������� ������ ��Ʈ���� ������
	@param[in] client ���۹��� ����
	@param[in] buff ������ ������
	@param[in] len ������ �������� ����
	*/
	void SendPacket(User* client, void* buff, size_t len);

	/**
	@brief Ÿ�̸� �̺�Ʈ�� �߰��Ѵ�.
	@param[in] client �̺�Ʈ�� ���
	@param[in] et �̺�Ʈ ����
	@param[in] delay_time �̺�Ʈ ��������� ������
	*/
	void AddEvent(size_t client, int et, size_t delay_time);

	/**
	@brief Ÿ�̸� �̺�Ʈ�� �߰��Ѵ�.
	@param[in] client �̺�Ʈ�� ���
	@param[in] et �̺�Ʈ ����
	@param[in] timePoint �̺�Ʈ ���� ����
	*/
	void AddEvent(size_t client, int et, std::chrono::high_resolution_clock::time_point timePoint);

	/**
	@brief ������ ������.
	*/
	void Run();
};