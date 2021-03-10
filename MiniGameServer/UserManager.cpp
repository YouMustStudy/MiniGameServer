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
		Logger::Log("처리되지 않은 유저 매니저 잡 발견");
		break;
	}
}

UserManager::UserManager()
{
	userIDSet.clear();
	queueType = { static_cast<size_t>(GlobalQueueType::USER_MANAGER), 0 };
	//인덱스 풀 초기화.
	for (size_t idx = 1; MAX_USER_SIZE >= idx ; ++idx)
		indexPool.push(MAX_USER_SIZE - idx);
	//유저 초기화
	for (size_t idx = 0; MAX_USER_SIZE > idx; ++idx)
		userList[idx].uid = idx;
}

void UserManager::ProcessAccept(AcceptInfo* info)
{
	if (nullptr == info)
	{
		Logger::Log("잘못된 ACCEPT - NULL 시도 발생");
		return;
	}

	if (true == indexPool.empty())
	{
		//할당할 수 있는 유저가 없다. -> 받지말자.
		Logger::Log("할당할 수 있는 유저가 없음");
		closesocket(info->socket);
		return;
	}

	size_t userIdx = indexPool.top(); 
	indexPool.pop();

	if (ST_DISCONN != userList[userIdx].state)
	{
		Logger::Log("재사용할 수 없는 유저 할당");
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
	Logger::Log("유저 접속 : " + std::string(nameBuf) + ":" + std::to_string(userList[userIdx].addr.sin_port));

	delete info;
}

void UserManager::ProcessDisconnect(size_t idx)
{
	//유저 상태 전환 후
	userList[idx].state = ST_DISCONN;

	//방에 있으면 방 나가기 처리.
	if (nullptr != userList[idx].roomPtr)
		userList[idx].roomPtr->PushJob(CS_LEAVEROOM, reinterpret_cast<void*>(&userList[idx]));
	else
		DisconnectUser(idx);
}

void UserManager::ProcessLogin(LoginInfo* info)
{
	if (nullptr == info)
	{
		Logger::Log("잘못된 로그인 요청 발생 - null");
		return;
	}

	size_t reqUser = info->idx;
	if (ST_NOLOGIN != userList[reqUser].state)
	{
		Logger::Log("잘못된 로그인 요청 발생 - 유저가 NOLOGIN 상태가 아님");
		return;
	}

	userList[reqUser].state = ST_NOLOGIN;
	//DB 아이디, 비밀번호 검증

	//중복 로그인 체크
	if (0 != userIDSet.count(info->id))
	{
		Logger::Log("중복 로그인 시도");
		SC_PACKET_LOGIN_FAIL packet(LOGIN_FAIL_SAME_ID);
		MiniGameServer::Instance().SendPacket(&userList[reqUser], &packet);
		return;
	}

	//로그인 OK!!
	userList[reqUser].id = info->id;
	userList[reqUser].state = ST_IDLE;
	userIDSet.emplace(info->id);
	Logger::Log("유저 로그인 성공");

	SC_PACKET_LOGIN_OK packet;
	MiniGameServer::Instance().SendPacket(&userList[reqUser], &packet);

	delete info;
}

void UserManager::ProcessEnqueue(size_t idx)
{
	//유저 상태체크 넣어야함
	if (ST_IDLE == userList[idx].state)
	{
		matchQueue.Enqueue(idx);
		userList[idx].state = ST_QUEUE;
		SC_PACKET_CHANGE_QUEUE packet{ true };
		MiniGameServer::Instance().SendPacket(&userList[idx], &packet);
		Logger::Log("매치큐 등록");

		if (true == matchQueue.CanMakeMake())
		{
			std::vector<size_t> matchUsers;
			if (false == matchQueue.MatchMake(matchUsers)) return;

			//룸매니저에게 통보.
			std::vector<User*> matchUserPtrs;
			for (auto userIdx : matchUsers)
			{
				matchUserPtrs.emplace_back(&userList[userIdx]);
				userList[userIdx].state = ST_PLAY;
			}
			RoomManager::Instance().PushJob(RMGR_CREATE, new CreateRoomInfo(matchUserPtrs));
			Logger::Log("인원수 충족, 룸 매니저에 방 생성 요청");
		}
	}
}

void UserManager::ProcessDequeue(size_t idx)
{
	//유저 상태체크 넣어야함
	if (ST_QUEUE == userList[idx].state)
	{
		matchQueue.Dequeue(idx);
		SC_PACKET_CHANGE_QUEUE packet{ false };
		MiniGameServer::Instance().SendPacket(&userList[idx], &packet);
		Logger::Log("매치큐 등록 해제");
	}
}

void UserManager::ProcessLeaveRoom(size_t idx)
{
	switch (userList[idx].state)
	{
	case ST_PLAY:
		//유저는 다시 로비로
		userList[idx].state = ST_IDLE;
		break;

	case ST_DISCONN:
		//세션 종료 등 마지막 최종 정리
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
		Logger::Log("유저 종료 : " + std::string(nameBuf) + ":" + std::to_string(userList[idx].addr.sin_port));
		indexPool.push(idx);
	}
}
