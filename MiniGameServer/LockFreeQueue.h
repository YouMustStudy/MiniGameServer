#pragma once
#include <concurrent_queue.h>
#include <atomic>

using QueueType = std::pair<size_t, size_t>;                 ///< LFQ ť Ÿ�� ������
extern Concurrency::concurrent_queue<QueueType> g_GlobalJob; ///< LFQ �۷ι� ť
extern thread_local bool TLS_isJob;                          ///< ���� �۾� ó�������� �Ǻ�

enum class GlobalQueueType : size_t
{
	USER_MANAGER,
	ROOM_MANAGER,
	ROOM
};

/**
*@brief �۾��� �񵿱������� ���޹ް�, �ϳ��� �����忡�� ó���ϴ� ���� �����ϴ� ��ü.
*@author Gurnwoo Kim.
*/
class LockFreeQueue
{
protected:
	using Job = std::pair<size_t, void*>;                 ///< �� �ڷ��� ����
	QueueType                          queueType{ 0, 0 }; ///< ť ����
	concurrency::concurrent_queue<Job> jobQueue;          ///< �۾� ť
	std::atomic_size_t                 jobCnt;            ///< �۾� ����

	/**
	*@brief �۾��� ó���Ѵ�. LFQ�� ��ӹ��� ��ü���� �ش� �Լ��� ���� �۾����� ó���Ѵ�.
	*@param[in] job �۾�.
	*/
	virtual void ProcessJob( Job /*job*/ ) { };

	/**
	*@brief �۷ι� ť�� �۾��� �����Ѵ�.
	*/
	void ProcessGlobalJob();

public:
	/**
	*@brief �Ҹ���.
	*/
	virtual ~LockFreeQueue() {};

	/**
	*@brief �۾��� �߰��Ѵ�.
	*@param[in] jobType �۾� ����.
	*@param[in] arg ����.
	*/
	void PushJob( size_t jobType, void* arg );

	/**
	*@brief ť�� �۾����� �����Ѵ�.
	*/
	void Flush();
};