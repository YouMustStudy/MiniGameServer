#include "DMRoom.h"
#include "..\Common\Client.h"
#include "..\Utills\Logger.h"
#include "..\protocol.h"
#include "..\RoomManager.h"
#include "..\MiniGameServer.h"

DMRoom::DMRoom()
{
}

DMRoom::~DMRoom()
{
}

void DMRoom::Init()
{
	lastUpdateTime = std::chrono::high_resolution_clock::now();
}

void DMRoom::Regist(std::vector<Client*> clients)
{
	//유저 등록 처리
	return;
}

void DMRoom::ProcessJob(Job job)
{
	switch (job.first)
	{
	case CS_UPDATE:
		Update();
		break;

	default:
		Logger::Log("처리되지 않은 룸 잡 발견");
		break;
	}
}

void DMRoom::Disconnect()
{
	//유저를 룸에서 삭제 및 유저 매니저에 통보.
}

void DMRoom::End()
{
	//유저 종료처리 및 룸 매니저에 통보
	RoomManager::Instance().PushJob(RMGR_DESTROY, reinterpret_cast<void*>(queueType.second));
}

void DMRoom::Update()
{
	currentUpdateTime = std::chrono::high_resolution_clock::now();
	delta_time = std::chrono::duration<float>(currentUpdateTime - lastUpdateTime).count();
	
    isEnd = GameLogic();
	SendGameState();
	if (true == isEnd)
		End();
	else
		MiniGameServer::Instance().AddEvent(queueType.second, EV_UPDATE, lastUpdateTime + std::chrono::milliseconds(UPDATE_INTERVAL));
	lastUpdateTime = currentUpdateTime;
}

void DMRoom::SetQueueType(QueueType queType)
{
	queueType = queType;
}

bool DMRoom::GameLogic()
{
	return true;
}

void DMRoom::SendGameState()
{
	//Send data to clients.
	eventData.Clear();
	infoData.Clear();
}