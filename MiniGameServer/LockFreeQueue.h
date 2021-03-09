#pragma once
#include <concurrent_queue.h>
#include <atomic>

using QueueType = std::pair<size_t, size_t>;

extern Concurrency::concurrent_queue<QueueType> g_GlobalJob;
extern thread_local bool TLS_isJob;

enum class GlobalQueueType : size_t
{
	USER_MANAGER,
	ROOM_MANAGER,
	ROOM
};

class LockFreeQueue
{
public:
	virtual ~LockFreeQueue() {};

	void PushJob(size_t jobType, void* arg);
	void Flush();

protected:
	using Job = std::pair<size_t, void*>;
	virtual void ProcessJob(Job job) { job.first; };  //�ǹ̾��� ����, =0�� ����ȭ �� �� array�� ��ü���� �Ұ����ϱ⿡ ��ü.
	void ProcessGlobalJob();

	QueueType queueType{0, 0};
	concurrency::concurrent_queue<Job> jobQueue;
	std::atomic_size_t jobCnt;
};