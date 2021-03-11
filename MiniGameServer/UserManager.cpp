#include "UserManager.h"
#include "RoomManager.h"
#include "MiniGameServer.h"


UserManager& UserManager::Instance()
{
	static UserManager* userManager = new UserManager();
	return *userManager;
}

void UserManager::ProcessJob(Job job)
{
	switch (job.first)
	{
	case USER_ACCEPT:
		ProcessAccept(reinterpret_cast<AcceptInfo*>(job.second));
		break;

	case USER_DISCONN:
		ProcessDisconnect(reinterpret_cast<size_t>(job.second));
		break;

	case USER_LOGIN:
		ProcessLogin(reinterpret_cast<LoginInfo*>(job.second));
		break;

	case USER_ENQUEUE:
		ProcessEnqueue(reinterpret_cast<size_t>(job.second));
		break;

	case USER_DEQUEUE:
		ProcessDequeue(reinterpret_cast<size_t>(job.second));
		break;

	case USER_LEAVEROOM:
		ProcessLeaveRoom(reinterpret_cast<size_t>(job.second));
		break;

	default:
		Logger::Log("ó������ ���� ���� �Ŵ��� �� �߰�");
		break;
	}
}

UserManager::UserManager()
{
	userIDSet.clear();
	queueType = { static_cast<size_t>(GlobalQueueType::USER_MANAGER), 0 };
	//�ε��� Ǯ �ʱ�ȭ.
	for (size_t idx = 1; MAX_USER_SIZE >= idx ; ++idx)
		indexPool.push(MAX_USER_SIZE - idx);
	//���� �ʱ�ȭ
	for (size_t idx = 0; MAX_USER_SIZE > idx; ++idx)
		userList[idx].uid = idx;
}

void UserManager::ProcessAccept(AcceptInfo* info)
{
	if (nullptr == info)
	{
		Logger::Log("�߸��� ACCEPT - NULL �õ� �߻�");
		return;
	}

	if (true == indexPool.empty())
	{
		//�Ҵ��� �� �ִ� ������ ����. -> ��������.
		Logger::Log("�Ҵ��� �� �ִ� ������ ����");
		closesocket(info->socket);
		return;
	}

	size_t userIdx = indexPool.top(); 
	indexPool.pop();

	if (ST_DISCONN != userList[userIdx].state)
	{
		Logger::Log("������ �� ���� ���� �Ҵ�");
		return;
	}

	userList[userIdx].socket = info->socket;
	userList[userIdx].addr = info->addr;
	userList[userIdx].state = ST_NOLOGIN;
	userList[userIdx].recvOver.Init(RECV_BUF_SIZE);
	userList[userIdx].recvOver.SetEvent(EV_RECV);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(userList[userIdx].socket), workerIOCP, userIdx, 0);
	userList[userIdx].SetRecv();

	SC_PACKET_CONNECT_OK packet;
	MiniGameServer::Instance().SendPacket(&userList[userIdx], &packet);

	char nameBuf[INET_ADDRSTRLEN]{0, };
	inet_ntop(AF_INET, &userList[userIdx].addr.sin_addr, nameBuf, INET_ADDRSTRLEN);
	Logger::Log("���� ���� : " + std::string(nameBuf) + ":" + std::to_string(userList[userIdx].addr.sin_port));

	delete info;
}

void UserManager::ProcessDisconnect(size_t idx)
{
	if (ST_QUEUE == userList[idx].state)
		matchQueue.Dequeue(idx);

	//���� ���� ��ȯ ��
	userList[idx].state = ST_DISCONN;

	//�濡 ������ �� ������ ó��.
	if (nullptr != userList[idx].roomPtr)
		userList[idx].roomPtr->PushJob(CS_LEAVEROOM, reinterpret_cast<void*>(&userList[idx]));
	else
		DisconnectUser(idx);
}

void UserManager::ProcessLogin(LoginInfo* info)
{
	if (nullptr == info)
	{
		Logger::Log("�߸��� �α��� ��û �߻� - null");
		return;
	}

	size_t reqUser = info->idx;
	if (ST_NOLOGIN != userList[reqUser].state)
	{
		Logger::Log("�߸��� �α��� ��û �߻� - ������ NOLOGIN ���°� �ƴ�");
		return;
	}

	userList[reqUser].state = ST_NOLOGIN;
	//DB ���̵�, ��й�ȣ ����

	//�ߺ� �α��� üũ
	/*if (0 != userIDSet.count(info->id))
	{
		Logger::Log("�ߺ� �α��� �õ�");
		SC_PACKET_LOGIN_FAIL packet(LOGIN_FAIL_SAME_ID);
		MiniGameServer::Instance().SendPacket(&userList[reqUser], &packet);
		return;
	}*/

	//�α��� OK!!
	userList[reqUser].id = info->id;
	userList[reqUser].state = ST_IDLE;
	userIDSet.emplace(info->id);
	Logger::Log("���� �α��� ����");

	SC_PACKET_LOGIN_OK packet;
	MiniGameServer::Instance().SendPacket(&userList[reqUser], &packet);

	delete info;
}

void UserManager::ProcessEnqueue(size_t idx)
{
	//���� ����üũ �־����
	if (ST_IDLE == userList[idx].state)
	{
		matchQueue.Enqueue(idx);
		userList[idx].state = ST_QUEUE;
		SC_PACKET_CHANGE_QUEUE packet{ true };
		MiniGameServer::Instance().SendPacket(&userList[idx], &packet);

		if (true == matchQueue.CanMakeMake())
		{
			std::vector<size_t> matchUsers;
			if (false == matchQueue.MatchMake(matchUsers)) return;

			//��Ŵ������� �뺸.
			std::vector<User*> matchUserPtrs;
			for (auto userIdx : matchUsers)
			{
				matchUserPtrs.emplace_back(&userList[userIdx]);
				userList[userIdx].state = ST_PLAY;
			}
			RoomManager::Instance().PushJob(RMGR_CREATE, new CreateRoomInfo(matchUserPtrs));
			Logger::Log("�ο��� ����, �� �Ŵ����� �� ���� ��û");
		}
	}
}

void UserManager::ProcessDequeue(size_t idx)
{
	//���� ����üũ �־����
	if (ST_QUEUE == userList[idx].state)
	{
		matchQueue.Dequeue(idx);
		SC_PACKET_CHANGE_QUEUE packet{ false };
		MiniGameServer::Instance().SendPacket(&userList[idx], &packet);
		Logger::Log("��ġť ��� ����");
	}
}

void UserManager::ProcessLeaveRoom(size_t idx)
{
	switch (userList[idx].state)
	{
	case ST_PLAY:
		//������ �ٽ� �κ��
		userList[idx].state = ST_IDLE;
		break;

	case ST_DISCONN:
		//���� ���� �� ������ ���� ����
		DisconnectUser(idx);
		break;
	}
}

void UserManager::DisconnectUser(size_t idx)
{
	if (ST_DISCONN == userList[idx].state)
	{
		closesocket(userList[idx].socket);
		userList[idx].socket = INVALID_SOCKET;
		userIDSet.erase(userList[idx].id);
		char nameBuf[INET_ADDRSTRLEN]{ 0, };
		inet_ntop(AF_INET, &userList[idx].addr.sin_addr, nameBuf, INET_ADDRSTRLEN);
		Logger::Log("���� ���� : " + std::string(nameBuf) + ":" + std::to_string(userList[idx].addr.sin_port));
		indexPool.push(idx);
	}
}
