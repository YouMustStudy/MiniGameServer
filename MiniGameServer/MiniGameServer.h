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
@brief 미니게임서버
@author Gurnwoo Kim
*/
class MiniGameServer
{
private:
	size_t NUM_THREADS{ 10 };
	short SERVER_PORT{ 15600 };

	std::atomic<int> m_playerNum;		///< 총 접속 인원 수
	HANDLE m_iocp;						///< 작업 스레드용 IOCP 핸들
	SOCKET m_listenSocket;				///< 리슨 소켓

	//타이머
	std::priority_queue<Event> m_timerQueue;	///< 타이머 큐
	std::mutex m_timerLock;						///< 타이머 큐 락
	std::vector<std::thread> m_threads;			///< 스레드 목록

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
	void AddTimer(Event& ev);

	/**
	@brief 도착한 패킷을 재조립한다.
	@param[in] client 수신한 대상
	@param[in] buffer 수신 버퍼
	@param[in] recvLength 수신한 크기
	*/
	void ParsePacket(size_t idx, User* client, void* buffer, size_t recvLength);

	/**
	@brief 패킷 해석 후 적절한 매니저에게 통보한다.
	@param[in] idx 수신한 유저의 인덱스
	@param[in] buffer 패킷이 담긴 버퍼
	*/
	void ProcessPacket(User* user, size_t idx, void* buffer);

	/**
	@brief 타이머 스레드 함수
	*/
	void TimerThread();
	/**
	@brief 작업 스레드 함수
	*/
	void WorkerThread();

	void PostEvent(size_t key, int eventType);
	void PostEvent(size_t key, int eventType, void* args);

	MiniGameServer();
public:
	static MiniGameServer& Instance();
	~MiniGameServer();

	/**
	@brief 유저에게 패킷 '하나'를 전송함
	@param[in] client 전송받을 유저
	@param[in] buff 전송할 데이터
	*/
	void SendPacket(User* client, void* buff);

	/**
	@brief 유저에게 데이터 스트림을 전송함
	@param[in] client 전송받을 유저
	@param[in] buff 전송할 데이터
	@param[in] len 전송할 데이터의 길이
	*/
	void SendPacket(User* client, void* buff, size_t len);

	/**
	@brief 타이머 이벤트를 추가한다.
	@param[in] client 이벤트의 대상
	@param[in] et 이벤트 종류
	@param[in] delay_time 이벤트 실행까지의 딜레이
	*/
	void AddEvent(size_t client, int et, size_t delay_time);

	/**
	@brief 타이머 이벤트를 추가한다.
	@param[in] client 이벤트의 대상
	@param[in] et 이벤트 종류
	@param[in] timePoint 이벤트 실행 시점
	*/
	void AddEvent(size_t client, int et, std::chrono::high_resolution_clock::time_point timePoint);

	/**
	@brief 서버를 시작함.
	*/
	void Run();
};