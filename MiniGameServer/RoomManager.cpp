#include "RoomManager.h"
#include "Utills/Logger.h"
#include "MiniGameServer.h"

RoomManager& RoomManager::Instance()
{
	static RoomManager* roomManager = new RoomManager();
	return *roomManager;
}

DMRoom* RoomManager::GetRoom(size_t idx)
{
	return &roomList[idx];
}

void RoomManager::ProcessJob(Job job)
{
	switch (job.first)
	{
	case RMGR_CREATE:
		ProcessCreateRoom(reinterpret_cast<CreateRoomInfo*>(job.second));
		break;

	case RMGR_DESTROY:
		ProcessDestroyRoom(reinterpret_cast<size_t>(job.second));
		break;

	default:
		Logger::Log("ó������ ���� �� �Ŵ��� �� �߰�");
		break;
	}
}

RoomManager::RoomManager()
{
	queueType = { static_cast<size_t>(GlobalQueueType::ROOM_MANAGER), 0 };
	//�ε��� Ǯ �ʱ�ȭ.
	for (size_t idx = 1; MAX_ROOM_SIZE >= idx; ++idx)
		indexPool.push(MAX_ROOM_SIZE - idx);

	for (size_t i = 0; i < MAX_ROOM_SIZE; ++i)
		roomList[i].SetQueueType({ static_cast<size_t>(GlobalQueueType::ROOM), i });
}

void RoomManager::ProcessCreateRoom(CreateRoomInfo* info)
{
	if (nullptr == info)
	{
		Logger::Log("�߸��� ����� - NULL �õ� �߻�");
		return;
	}

	if (true == indexPool.empty())
	{
		//�Ҵ��� �� �ִ� ���� ����. -> ��������.
		Logger::Log("�Ҵ��� �� �ִ� ���� ����");
		return;
	}

	size_t roomIdx = indexPool.top();
	indexPool.pop();

	roomList[roomIdx].Init();
	roomList[roomIdx].Regist(info->users);

	MiniGameServer::Instance().AddEvent(roomIdx, EV_UPDATE, std::chrono::high_resolution_clock::now());
	delete info;
}

void RoomManager::ProcessDestroyRoom(size_t idx)
{
	if (true == roomList[idx].IsEnd())
		indexPool.push(idx);
}
