#pragma once
#include <concurrent_queue.h>
#include <atomic>

extern Concurrency::concurrent_queue<size_t> g_GlobalJob;
extern thread_local bool TLS_isJob;

enum class GlobalQueueType : size_t
{
	USER_MANAGER,
	ROOM_MANAGER
};

class LockFreeQueue
{
public:
	virtual ~LockFreeQueue() {};

	void PushJob(size_t jobType, void* arg);
	void Flush();

protected:
	using Job = std::pair<size_t, void*>;
	virtual void ProcessJob(Job job) = 0;
	void ProcessGlobalJob();

	size_t queueType{};
	concurrency::concurrent_queue<Job> jobQueue;
	std::atomic_size_t jobCnt;
};