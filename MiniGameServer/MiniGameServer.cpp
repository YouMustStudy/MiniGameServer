#include "MiniGameServer.h"
#include "UserManager.h"
#include "RoomManager.h"

using namespace std;
using namespace chrono;

MiniGameServer::MiniGameServer() :
	m_listenSocket(INVALID_SOCKET)
{
	Logger::Log("[Minigame Ser-ver]");
	LoadConfig();
	InitRooms();
	InitWSA();
	InitThreads();
}
MiniGameServer& MiniGameServer::Instance()
{
	static MiniGameServer* miniGameServer = new MiniGameServer();
	return *miniGameServer;
}
MiniGameServer::~MiniGameServer()
{
	for (UINT i = 0; i < m_threads.size(); ++i)
		m_threads[i].join();
	closesocket(m_listenSocket);
	WSACleanup();
}

void MiniGameServer::InitWSA()
{
	WSADATA wsa;
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsa))
		Logger::WsaLog("Error at WSAStartup()", WSAGetLastError());

	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 0);
	if(INVALID_HANDLE_VALUE == m_iocp)
		Logger::WsaLog("Error at Create IOCP", WSAGetLastError());
	UserManager::Instance().SetIOCPHandle(m_iocp);

	m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_listenSocket)
		Logger::WsaLog("Error at Create ListenSocket", WSAGetLastError());

	CSOCKADDR_IN serverAddr{ INADDR_ANY, SERVER_PORT };

	Logger::Log("Initializing ListenSocket...");
	if (SOCKET_ERROR == ::bind(m_listenSocket, serverAddr.GetSockAddr(), *serverAddr.Len()))
		Logger::WsaLog("Error at Bind()", WSAGetLastError());
	if (SOCKET_ERROR == ::listen(m_listenSocket, SOMAXCONN))
		Logger::WsaLog("Error at Listen()", WSAGetLastError());
}

void MiniGameServer::InitThreads()
{
	Logger::Log("Initializing Threads...");
	for (size_t i = 0; i < NUM_THREADS; ++i)
		m_threads.emplace_back(&MiniGameServer::WorkerThread, this);
	m_threads.emplace_back(&MiniGameServer::TimerThread, this);
}

void MiniGameServer::InitRooms()
{
	Logger::Log("Initializing Rooms...");
}

void MiniGameServer::LoadConfig()
{
	Logger::Log("Load Config...");
}

void MiniGameServer::Run()
{
	Logger::Log("[SERVER ONLINE]");
	CSOCKADDR_IN clientAddr{};
	SOCKET clientSocket{};
	while (true)
	{
		clientSocket = accept(m_listenSocket, clientAddr.GetSockAddr(), clientAddr.Len());
		if (INVALID_SOCKET != clientSocket)
			PostEvent(0, EV_ACCEPT, new AcceptInfo{clientSocket, *reinterpret_cast<SOCKADDR_IN*>(clientAddr.GetSockAddr())});
	}
}
void MiniGameServer::WorkerThread()
{
	DWORD ReceivedBytes;
	ULONGLONG key;
	OVERLAPPED* over;

	while (true)
	{
		BOOL success = GetQueuedCompletionStatus(m_iocp, &ReceivedBytes, &key, &over, INFINITE);

		OverEx* overEx = reinterpret_cast<OverEx*>(over);
		User* client = reinterpret_cast<User*>(overEx);

		if (nullptr == client || nullptr == overEx)
			continue;

		if (false == success || 0 == ReceivedBytes) {
			if (EV_SEND == overEx->EventType())
				delete overEx;
			else if(EV_RECV == overEx->EventType())
				UserManager::Instance().PushJob(USER_DISCONN, reinterpret_cast<void*>(key));
			continue;
		}

		switch (overEx->EventType())
		{
		case EV_ACCEPT:
			UserManager::Instance().PushJob(USER_ACCEPT, overEx->Overlapped()->hEvent);
			break;

		case EV_RECV:
			ParsePacket(key, client, overEx->Data(), ReceivedBytes);
			client->SetRecv();
			break;

		case EV_UPDATE:
		{
			auto roomPtr = RoomManager::Instance().GetRoom(key);
			if (nullptr != roomPtr)
				roomPtr->PushJob(CS_UPDATE, nullptr);
			delete overEx;
			break;
		}

		case EV_SEND:
			delete overEx;
			break;

		default:
			std::wcout << "Unknown Event From " << client << std::endl;
			while (true);
			break;
		}
	}
}
void MiniGameServer::ProcessPacket(User* user, size_t idx, void* buffer)
{
	if (nullptr == buffer) return;

	auto dp = reinterpret_cast<DEFAULT_PACKET*>(buffer);
	switch (dp->type)
	{
	case CS_REQUEST_LOGIN:
	{
		auto packet = reinterpret_cast<CS_PACKET_REQUEST_LOGIN*>(buffer);
		UserManager::Instance().PushJob(USER_LOGIN, new LoginInfo(idx, reinterpret_cast<const char*>(packet->name), packet->characterType));
		break;
	}
	case CS_ENQUEUE:
		UserManager::Instance().PushJob(USER_ENQUEUE, reinterpret_cast<void*>(idx));
		break;

	case CS_DEQUEUE:
		UserManager::Instance().PushJob(USER_DEQUEUE, reinterpret_cast<void*>(idx));
		break;

	case CS_ATTACK:
	{
		if (nullptr != user->roomPtr)
		{
			auto packet = reinterpret_cast<CS_PACKET_ATTACK*>(buffer);
			user->roomPtr->PushJob(CS_ATTACK, reinterpret_cast<void*>(packet->uid));
		}
		break;
	}
	case CS_MOVEDIR:
	{
		if (nullptr != user->roomPtr)
		{
			auto packet = reinterpret_cast<CS_PACKET_MOVEDIR*>(buffer);
			user->roomPtr->PushJob(CS_MOVEDIR, reinterpret_cast<void*>(
				new MoveDirInfo(packet->uid, packet->dx, packet->dy)
				));
		}
		break;
	}
	case CS_READY:
	{
		if (nullptr != user->roomPtr)
		{
			auto packet = reinterpret_cast<CS_PACKET_MOVEDIR*>(buffer);
			user->roomPtr->PushJob(CS_READY, reinterpret_cast<void*>(packet->uid));
		}
		break;
	}
	
	default:
		Logger::Log("처리되지 않은 유저 패킷 수신" + std::to_string(dp->type));
		break;
	}
}
void MiniGameServer::TimerThread()
{
	while (true) {
		m_timerLock.lock();
		if (true == m_timerQueue.empty()) {
			m_timerLock.unlock();
			this_thread::sleep_for(3ms);
			continue;
		}

		const Event& ev = m_timerQueue.top();

		if (ev.wakeupTime > high_resolution_clock::now()) {
			m_timerLock.unlock();
			this_thread::sleep_for(3ms);
			continue;
		}
		Event p_ev = ev;
		m_timerQueue.pop();
		m_timerLock.unlock();

		PostEvent(p_ev.targetID, p_ev.eventType);
	}
}
void MiniGameServer::AddTimer(Event& ev)
{
	m_timerLock.lock();
	m_timerQueue.push(ev);
	m_timerLock.unlock();
}
void MiniGameServer::AddEvent(size_t client, int et, size_t milisec_delay)
{
	Event ev{ client, et, high_resolution_clock::now() + chrono::milliseconds(milisec_delay) };
	AddTimer(ev);
}
void MiniGameServer::AddEvent(size_t client, int et, high_resolution_clock::time_point timePoint)
{
	Event ev{ client, et, timePoint };
	AddTimer(ev);
}
void MiniGameServer::ParsePacket(size_t idx, User* client, void* buffer, size_t recvLength)
{
	if (nullptr == client || nullptr == buffer)
		return;

	size_t copySize = 0;
	char* bufPos = reinterpret_cast<char*>(buffer);
	size_t& savedSize = client->savedPacket.len;
	size_t& needSize = client->needSize;

	while (0 < recvLength) {
		if (recvLength + savedSize >= needSize) 
		{
			copySize = needSize - savedSize;
			client->savedPacket.EmplaceBack(bufPos, copySize);

			if (sizeof(PACKET_SIZE) == needSize) 
			{
				needSize = *reinterpret_cast<PACKET_SIZE*>(client->savedPacket.data);
				savedSize -= copySize;
				continue;
			}

			//패킷 처리
			ProcessPacket(client, idx, client->savedPacket.data);

			savedSize = 0;
			bufPos += copySize;
			recvLength -= copySize;
			needSize = sizeof(PACKET_SIZE);
		}
		else
		{
			client->savedPacket.EmplaceBack(bufPos, recvLength);
			recvLength = 0;
		}
	}
}
void MiniGameServer::SendPacket(User* client, void* buff)
{
	if (nullptr != client && nullptr != buff)
	{
		if (INVALID_SOCKET != client->socket)
		{
			OverEx* sendOver = new OverEx{ EV_SEND, buff };
			if(nullptr != sendOver)
				WSASend(client->socket, sendOver->Buffer(), 1, 0, 0, sendOver->Overlapped(), 0);
		}
	}
}
void MiniGameServer::SendPacket(User* client, void* buff, size_t len)
{
	if (nullptr != client && nullptr != buff)
	{
		if (INVALID_SOCKET != client->socket)
		{
			OverEx* sendOver = new OverEx{ EV_SEND, buff, len };
			if (nullptr != sendOver)
				WSASend(client->socket, sendOver->Buffer(), 1, 0, 0, sendOver->Overlapped(), 0);
		}
	}
}
void MiniGameServer::PostEvent(size_t key, int eventType)
{
	OverEx* overEx = new OverEx;
	if (nullptr != overEx)
	{
		::memset(overEx, 0, sizeof(OverEx));
		overEx->SetEvent(eventType);
		PostQueuedCompletionStatus(m_iocp, 1, key, overEx->Overlapped());
	}
}
void MiniGameServer::PostEvent(size_t key, int eventType, void* args)
{
	OverEx* overEx = new OverEx;
	if (nullptr != overEx)
	{
		::memset(overEx, 0, sizeof(OverEx));
		overEx->SetEvent(eventType);
		overEx->Overlapped()->hEvent = args;
		PostQueuedCompletionStatus(m_iocp, 1, key, overEx->Overlapped());
	}
}
