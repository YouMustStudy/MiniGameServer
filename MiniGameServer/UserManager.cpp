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

	default:
		Logger::Log("ó������ ���� ���� �Ŵ��� �� �߰�");
		break;
	}
}

UserManager::UserManager()
{
	queueType = { static_cast<size_t>(GlobalQueueType::USER_MANAGER), 0 };
	//�ε��� Ǯ �ʱ�ȭ.
	for (size_t idx = 1; MAX_USER_SIZE >= idx ; ++idx)
		indexPool.push(MAX_USER_SIZE - idx);
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

	userList[userIdx].socket = info->socket;
	userList[userIdx].addr = info->addr;
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
	//�� ���� �߰�

	closesocket(userList[idx].socket);
	userList[idx].socket = INVALID_SOCKET;

	char nameBuf[INET_ADDRSTRLEN]{ 0, };
	inet_ntop(AF_INET, &userList[idx].addr.sin_addr, nameBuf, INET_ADDRSTRLEN);
	Logger::Log("���� ���� : " + std::string(nameBuf) + ":" + std::to_string(userList[idx].addr.sin_port));

	indexPool.push(idx);
}

void UserManager::ProcessLogin(LoginInfo* info)
{
	if (nullptr == info)
	{
		Logger::Log("�߸��� �α��� �õ� �߻�");
		return;
	}

	size_t reqUser = info->idx;
	//DB ���̵�, ��й�ȣ ����

	//�ߺ� �α��� üũ
	if (0 != userIDSet.count(info->id))
	{
		SC_PACKET_LOGIN_FAIL packet;
		MiniGameServer::Instance().SendPacket(&userList[reqUser], &packet);
		return;
	}

	//�α��� OK!!
	userList[reqUser].id = info->id;
	userIDSet.emplace(info->id);

	SC_PACKET_LOGIN_OK packet;
	MiniGameServer::Instance().SendPacket(&userList[reqUser], &packet);

	delete info;
}

void UserManager::ProcessEnqueue(size_t idx)
{
	//���� ����üũ �־����
	matchQueue.Enqueue(idx);
	SC_PACKET_CHANGE_QUEUE packet{ true };
	MiniGameServer::Instance().SendPacket(&userList[idx], &packet);

	if (true == matchQueue.CanMakeMake())
	{
		std::vector<size_t> matchUsers;
		if (false == matchQueue.MatchMake(matchUsers)) return;

		//��Ŵ������� �뺸.
		std::vector<Client*> matchUserPtrs;
		for (auto userIdx : matchUsers)
			matchUserPtrs.emplace_back(&userList[userIdx]);
		RoomManager::Instance().PushJob(RMGR_CREATE, new CreateRoomInfo(matchUserPtrs));
	}
}

void UserManager::ProcessDequeue(size_t idx)
{
	//���� ����üũ �־����
	matchQueue.Dequeue(idx);
	SC_PACKET_CHANGE_QUEUE packet{ false };
	MiniGameServer::Instance().SendPacket(&userList[idx], &packet);
}
