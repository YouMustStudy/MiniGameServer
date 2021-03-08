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
@brief 미니게임서버
@author Gurnwoo Kim
*/
class MiniGameServer
{
private:
	int UPDATE_INTERVAL{ 20 };		///< Interval of updateing time. Millisecond.
	int NUM_THREADS{ 6 };
	short SERVER_PORT{ 15600 };

	std::atomic<int> playerNum;		///< 총 접속 인원 수
	HANDLE m_iocp;					///< 작업 스레드용 IOCP 핸들
	SOCKET m_listenSocket;			///< 리슨 소켓

	DMRoom m_room;							///< 게임이 실행되는 단위

	//타이머
	std::priority_queue<Event> timer_queue; ///< 타이머 큐
	std::mutex timer_lock;					///< 타이머 큐 락
	std::vector<std::thread> m_threads;		///< 스레드 목록

	/**
	@brief WSA 초기화.
	*/
	void InitWSA();

	/**
	@brief 타이머, 작업 스레드 초기화.
	*/
	void InitThreads();

	/**
	@brief 방 초기화.
	*/
	void InitRooms();

	/**
	@brief 옵션값을 읽어온다. - 아직 미구현
	*/
	void LoadConfig();

	/**
	@brief 타이머 이벤트를 추가한다.
	@param[in] ev 추가할 이벤트
	*/
	void add_timer(Event& ev);

	/**
	@brief 타이머 이벤트를 추가한다.
	@param[in] client 이벤트의 대상
	@param[in] et 이벤트 종류
	@param[in] delay_time 이벤트 실행까지의 딜레이
	*/
	void AddEvent(int client, EVENT_TYPE et, int delay_time);

	/**
	@brief 도착한 패킷을 재조립한다.
	@param[in] client 수신한 대상
	@param[in] buffer 수신 버퍼
	@param[in] recvLength 수신한 크기
	*/
	void ParsePacket(Client* client, void* buffer, size_t recvLength);

	/**
	@brief 타이머 스레드 함수
	*/
	void TimerThread();
	/**
	@brief 작업 스레드 함수
	*/
	void WorkerThread();

	/**
	@brief 플레이어 종료 처리
	*/
	void DisconnectPlayer(Client* client);

	/**
	@brief 유저에게 패킷을 전송함
	@param[in] client 전송받을 유저
	@param[in] buff 전송할 데이터
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
	@brief 서버를 시작함.
	*/
	void Run();
};