#include "MiniGameServer.h"



using namespace std;
using namespace chrono;

enum EVENT_TYPE { EV_RECV, EV_SEND, EV_UPDATE };
enum CL_STATE { ST_IDLE, ST_QUEUE, ST_PLAY };

MiniGameServer::MiniGameServer() :
	m_listenSocket(INVALID_SOCKET)
{
	Logger::Log("[Minigame Ser-ver]");
	LoadConfig();
	InitRooms();
	InitWSA();
	InitThreads();
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

	m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_listenSocket)
		Logger::WsaLog("Error at Create ListenSocket", WSAGetLastError());

	CSOCKADDR_IN serverAddr{ INADDR_ANY, SERVER_PORT };

	Logger::Log("Initializing ListenSocket...");
	if (SOCKET_ERROR == ::bind(m_listenSocket, serverAddr.GetSockAddr(), *serverAddr.Len()))
		Logger::WsaLog("Error at Bind()", WSAGetLastError());
	if (SOCKET_ERROR == ::listen(m_listenSocket, SOMAXCONN))
		Logger::WsaLog("Error at Listen()", WSAGetLastError());

	//CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_Lobby.socket), m_iocp, reinterpret_cast<ULONG_PTR>(&m_Lobby), 0);
}

void MiniGameServer::InitThreads()
{
	Logger::Log("Initializing Threads...");
	for (UINT i = 0; i < NUM_THREADS; ++i)
		m_threads.emplace_back(&MiniGameServer::WorkerThread, this);
	m_threads.emplace_back(&MiniGameServer::TimerThread, this);
}

void MiniGameServer::InitRooms()
{
	Logger::Log("Initializing Rooms...");
	m_room.Init();
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
		//클라이언트 생성
		clientSocket = accept(m_listenSocket, clientAddr.GetSockAddr(), clientAddr.Len());
		Client* client = new Client;
		client->socket = clientSocket;
		client->recvOver.Init(RECV_BUF_SIZE);
		client->recvOver.SetEvent(EV_RECV);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client->socket), m_iocp, reinterpret_cast<ULONG_PTR>(client), 0);
		client->SetRecv();
		std::wcout << L"[CLIENT - " << clientSocket << L"] Accept" << std::endl;
	}
}
void MiniGameServer::WorkerThread()
{
	DWORD ReceivedBytes;
	ULONGLONG key;
	OVERLAPPED* over;

	while (true)
	{
		GetQueuedCompletionStatus(m_iocp, &ReceivedBytes, &key, &over, INFINITE);

		Client* client = reinterpret_cast<Client*>(key);
		OverEx* overEx = reinterpret_cast<OverEx*>(over);
		if (nullptr == client)
			continue;
		if (nullptr == overEx)
			continue;

		if (0 == ReceivedBytes) {
			if (EV_SEND == overEx->EventType())
				delete overEx;
			else if(EV_RECV == overEx->EventType())
				DisconnectPlayer(client);
			continue;
		}

		switch (overEx->EventType())
		{
		case EV_RECV:
			ParsePacket(client, overEx->Data(), ReceivedBytes);
			client->SetRecv();
			break;

		case EV_UPDATE:
		{
			m_room.update(0.0f);
			AddEvent(static_cast<int>(key), EV_UPDATE, UPDATE_INTERVAL);
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

void MiniGameServer::DisconnectPlayer(Client* client)
{
	if (nullptr != client)
	{
		if (client->socket != INVALID_SOCKET) {
			closesocket(client->socket);
			client->socket = INVALID_SOCKET;
		}
		delete client;
	}
}

void MiniGameServer::TimerThread()
{
	while (true) {
		timer_lock.lock();
		if (true == timer_queue.empty()) {
			timer_lock.unlock();
			this_thread::sleep_for(3ms);
			continue;
		}

		const Event& ev = timer_queue.top();

		if (ev.wakeupTime > high_resolution_clock::now()) {
			timer_lock.unlock();
			this_thread::sleep_for(3ms);
			continue;
		}
		Event p_ev = ev;
		timer_queue.pop();
		timer_lock.unlock();

		OverEx* overEx = new OverEx;
		::memset(overEx, 0, sizeof(OverEx));
		overEx->SetEvent(p_ev.eventType);

		PostQueuedCompletionStatus(m_iocp, 1, p_ev.targetID, overEx->Overlapped());
	}
}
void MiniGameServer::add_timer(Event& ev) 
{
	timer_lock.lock();
	timer_queue.push(ev);
	timer_lock.unlock();
}
void MiniGameServer::AddEvent(int client, EVENT_TYPE et, int milisec_delay)
{
	Event ev{ client, et, high_resolution_clock::now() + chrono::milliseconds(milisec_delay) };
	add_timer(ev);
}

void MiniGameServer::ParsePacket(Client* client, void* buffer, size_t recvLength)
{
	size_t copySize = 0;
	char* buf_ptr = reinterpret_cast<char*>(buffer);
	size_t& savedSize = client->savedSize;
	size_t& needSize = client->needSize;
	while (0 < recvLength) {
		if (recvLength + savedSize >= needSize) {
			copySize = needSize - savedSize;
			client->savedPacket.EmplaceBack(buf_ptr, copySize);
			if (sizeof(DEFAULT_PACKET) == needSize) {
				needSize = reinterpret_cast<DEFAULT_PACKET*>(client->savedPacket.data)->size;
				savedSize -= copySize;
				continue;
			}
			//패킷 처리하기
			buf_ptr += copySize;
			recvLength -= copySize;
			needSize = sizeof(DEFAULT_PACKET);
		}
		else {
			client->savedPacket.EmplaceBack(buf_ptr, recvLength);
			recvLength = 0;
		}
	}
}

void MiniGameServer::send_packet(Client* client, void* buff)
{
	if (nullptr != client &&
		nullptr != buff)
	{
		OverEx* sendOver = new OverEx{ EV_SEND, buff };
		WSASend(client->socket, sendOver->Buffer(), 1, 0, 0, sendOver->Overlapped(), 0);
	}
}
void MiniGameServer::send_packet_default(Client* client, int TYPE)
{
	if (nullptr != client)
	{
		DEFAULT_PACKET dp;
		send_packet(client, &dp);
	}
}