#include "LockFreeQueue.h"
Concurrency::concurrent_queue<size_t> g_GlobalJob = Concurrency::concurrent_queue<size_t>();
thread_local bool TLS_isJob = false;

void LockFreeQueue::PushJob(size_t jobType, void* arg)
{
	if (0 != jobCnt.fetch_add(1))
	{
		jobQueue.push(Job(jobType, arg));
	}
	else
	{
		jobQueue.push(Job(jobType, arg));
		if (true == TLS_isJob)
			g_GlobalJob.push(queueType);
		else
		{
			Flush();
			ProcessGlobalJob();
		}
	}
}

void LockFreeQueue::Flush()
{
	TLS_isJob = true;

	size_t remainJob = jobCnt;
	size_t left{};
	Job newJob;
	do {
		for (left = 0; left < remainJob; ++left)
		{
			while (false == jobQueue.try_pop(newJob)) {};
			ProcessJob(newJob);
		}
	} while (remainJob != jobCnt.fetch_sub(remainJob));

	TLS_isJob = false;
}

void LockFreeQueue::ProcessGlobalJob()
{
	while (false == g_GlobalJob.empty())
	{
		size_t jobTarget{};
		if (true == g_GlobalJob.try_pop(jobTarget))
		{
			switch (GlobalQueueType(jobTarget))
			{
			case GlobalQueueType::USER_MANAGER:
				break;

			case GlobalQueueType::ROOM_MANAGER:
				break;
			}
		}
	}
}
