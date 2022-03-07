#pragma once
#include <concurrent_queue.h>
#include <atomic>

using QueueType = std::pair<size_t, size_t>;                 ///< LFQ 큐 타입 재정의
extern Concurrency::concurrent_queue<QueueType> g_GlobalJob; ///< LFQ 글로벌 큐
extern thread_local bool TLS_isJob;                          ///< 현재 작업 처리중인지 판별

enum class GlobalQueueType : size_t
{
	USER_MANAGER,
	ROOM_MANAGER,
	ROOM
};

/**
*@brief 작업을 비동기적으로 전달받고, 하나의 스레드에서 처리하는 것을 보장하는 객체.
*@author Gurnwoo Kim.
*/
class LockFreeQueue
{
protected:
	using Job = std::pair<size_t, void*>;                 ///< 잡 자료형 정의
	QueueType                          queueType{ 0, 0 }; ///< 큐 종류
	concurrency::concurrent_queue<Job> jobQueue;          ///< 작업 큐
	std::atomic_size_t                 jobCnt;            ///< 작업 갯수

	/**
	*@brief 작업을 처리한다. LFQ를 상속받은 객체들은 해당 함수를 통해 작업들을 처리한다.
	*@param[in] job 작업.
	*/
	virtual void ProcessJob( Job /*job*/ ) { };

	/**
	*@brief 글로벌 큐의 작업을 수행한다.
	*/
	void ProcessGlobalJob();

public:
	/**
	*@brief 소멸자.
	*/
	virtual ~LockFreeQueue() {};

	/**
	*@brief 작업을 추가한다.
	*@param[in] jobType 작업 종류.
	*@param[in] arg 인자.
	*/
	void PushJob( size_t jobType, void* arg );

	/**
	*@brief 큐의 작업들을 수행한다.
	*/
	void Flush();
};