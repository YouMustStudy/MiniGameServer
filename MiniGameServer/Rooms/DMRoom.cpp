#include "DMRoom.h"
#include "..\Common\User.h"
#include "..\Utills\Logger.h"
#include "..\protocol.h"

#include "..\RoomManager.h"
#include "..\UserManager.h"
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

void DMRoom::Regist(std::vector<User*> users)
{
	//���� ��� ó��
	for (auto& user : users)
	{
		if (nullptr != user)
		{
			user->roomPtr = this;
		}
	}
	return;
}

void DMRoom::ProcessJob(Job job)
{
	switch (job.first)
	{
	case CS_UPDATE:
		Update();
		break;

	case CS_LEAVEROOM:
		Disconnect(reinterpret_cast<User*>(job.second));
		break;

	default:
		Logger::Log("ó������ ���� �� �� �߰�");
		break;
	}
}

void DMRoom::Disconnect(User* user)
{
	//������ �뿡�� ���� �� ���� �Ŵ����� �뺸.
	if (nullptr != user)
	{
		user->roomPtr = nullptr;
		UserManager::Instance().PushJob(USER_LEAVEROOM, reinterpret_cast<void*>(user->uid));
	}
}

void DMRoom::End()
{
	//���� ����ó�� �� �� �Ŵ����� �뺸
	RoomManager::Instance().PushJob(RMGR_DESTROY, reinterpret_cast<void*>(queueType.second));
}

void DMRoom::Update()
{
	currentUpdateTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(currentUpdateTime - lastUpdateTime).count();
	
    isEnd = GameLogic();
	SendGameState();
	Logger::Log("�� ������Ʈ ���� - " + std::to_string(deltaTime) + "ms");
	if (true == isEnd)
	{
		End();
		Logger::Log("��ġ ����, �� �Ŵ����� ���� ��û");
	}
	else
	{
		MiniGameServer::Instance().AddEvent(queueType.second, EV_UPDATE, lastUpdateTime + std::chrono::milliseconds(UPDATE_INTERVAL));
	}
	lastUpdateTime = currentUpdateTime;
}

void DMRoom::SetQueueType(QueueType queType)
{
	queueType = queType;
}

bool DMRoom::GameLogic()
{
	left_time -= deltaTime;
	if (0 >= left_time)
		return true;
	return false;
}

void DMRoom::SendGameState()
{
	//Send data to clients.
	eventData.Clear();
	infoData.Clear();
}