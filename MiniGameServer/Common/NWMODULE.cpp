#include "NWMODULE.h"
#include <iostream>
#include <string>
#include <fstream>

template <class T>
void NWMODULE<T>::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"¿¡·¯ " << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}


template <class T>
void NWMODULE<T>::InitWSA()
{
	WSADATA wsa;
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsa))
		error_display("Error at WSAStartup()", WSAGetLastError());
}

template<class T>
void NWMODULE<T>::InitConfig()
{

	std::ifstream ini{ config_path.c_str() };
	if (false == ini.is_open())
		gen_default_config();
	ini.close();

	wchar_t buffer[512];
	GetPrivateProfileString(L"CLIENT", L"LOBBY_PORT", L"15500", buffer, 512, config_path.c_str());
	lobby_port = std::stoi(buffer);

	GetPrivateProfileString(L"CLIENT", L"LOBBY_ADDR", L"127.0.0.1", buffer, 512, config_path.c_str());
	lobby_addr = std::wstring(buffer);

	GetPrivateProfileString(L"CLIENT", L"BATTLE_PORT", L"15600", buffer, 512, config_path.c_str());
	battle_port = std::stoi(buffer);

	GetPrivateProfileString(L"CLIENT", L"BATTLE_ADDR", L"127.0.0.1", buffer, 512, config_path.c_str());
	battle_addr = std::wstring(buffer);
}


template <class T>
void NWMODULE<T>::send_default_packet(int type)
{
	packet_inheritance cdp;
	cdp.size = sizeof(packet_inheritance);
	cdp.type = type;
	send(lobby_socket, reinterpret_cast<const char*>(&cdp), cdp.size, 0);
}


template <class T>
bool NWMODULE<T>::connect_server(SOCKET& socket, const wchar_t* address, const short port)
{
	socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socket) {
		error_display("Error at WSASocketW()", WSAGetLastError());
		return false;
	}
	CSOCKADDR_IN serverAddr{ address, port };
	int retval = ::connect(socket, serverAddr.GetSockAddr(), *serverAddr.Len());
	if (SOCKET_ERROR == retval) {
		error_display("Error at Connect()", WSAGetLastError());
		return false;
	}
	return true;
}


template<class T>
void NWMODULE<T>::packet_drain(PACKET_BUFFER& packet_buffer, char* buffer, size_t len)
{
	size_t& min_size = packet_buffer.min_size;
	size_t& need_size = packet_buffer.need_size;
	size_t& saved_size = packet_buffer.d_packet.len;
	PacketVector& d_packet = packet_buffer.d_packet;
	PacketVector& s_packet = packet_buffer.s_packet;

	size_t copy_size = 0;
	char* buf_ptr = reinterpret_cast<char*>(buffer);

	while (0 < len) {
		if (len + saved_size >= need_size) {
			copy_size = need_size - saved_size;
			d_packet.emplace_back(buf_ptr, copy_size);
			if (need_size == min_size) {
				need_size = *reinterpret_cast<PACKET_SIZE*>(d_packet.data);
				saved_size -= copy_size;
				continue;
			}
			s_packet.emplace_back(d_packet.data, d_packet.len);
			buf_ptr += copy_size;
			len -= copy_size;
			need_size = min_size; d_packet.clear();
		}
		else {
			d_packet.emplace_back(buf_ptr, len);
			len = 0;
		}
	}
	packet_buffer.SavedtoComplete();
}


template<class T>
void NWMODULE<T>::process_disconnect(SOCKET& socket, PACKET_BUFFER& buffer)
{
	if (socket != INVALID_SOCKET) {
		closesocket(socket);
		socket = INVALID_SOCKET;
	}

	buffer.d_packet.clear();
	/*buffer.c_lock.lock();
	buffer.c_packet.clear();
	buffer.c_lock.unlock();*/
}


template <class T>
void NWMODULE<T>::process_packet(int packet_type, void* buffer)
{
	callbacks[packet_type](MainModule, reinterpret_cast<packet_inheritance*>(buffer));
}

template<class T>
void NWMODULE<T>::gen_default_config()
{
	WritePrivateProfileString(L"CLIENT", L"LOBBY_PORT", L"15500", config_path.c_str());
	WritePrivateProfileString(L"CLIENT", L"LOBBY_ADDR", L"127.0.0.1", config_path.c_str());
	WritePrivateProfileString(L"CLIENT", L"BATTLE_PORT", L"15600", config_path.c_str());
	WritePrivateProfileString(L"CLIENT", L"BATTLE_ADDR", L"127.0.0.1", config_path.c_str());
}

template<class T>
void NWMODULE<T>::send_packet(packet_inheritance* packet)
{
	//Send battle or lobby after check packet_type.
	SOCKET socket{INVALID_SOCKET};
	if (packet->type < SSCS_TO_LOBBY_TO_BATTLE)
		socket = lobby_socket;
	else
		socket = battle_socket;

	if (iocp != INVALID_HANDLE_VALUE) {
		OverEx* send_over = new OverEx{ ev_send, packet };
		WSASend(socket, send_over->buffer(), 1, 0, 0, send_over->overlapped(), 0);
	}
	else {
		send(socket, reinterpret_cast<const char*>(packet), packet->size, 0);
	}
}

template<class T>
void NWMODULE<T>::destroy()
{
	// SD_BOTH(2): Shutdown both send and receive operations.
	closesocket(lobby_socket);
	closesocket(battle_socket);

	for (int i = 0; i < threads.size(); ++i)
		threads[i].join();
	threads.clear();
	WSACleanup();
}

template <class T>
void NWMODULE<T>::RecvLobbyThread()
{
	int received_size = 0;
	while (true)
	{
		received_size = recv(lobby_socket, lobby_over.data(), MAX_BUFFER_SIZE, 0);
		if (received_size == SOCKET_ERROR)
		{
			error_display("RECV ERROR ", WSAGetLastError());
			return;
		}
		if (received_size == 0) {
			cout << "[Connection Closed]" << endl;
			return;
		}
		packet_drain(lobby_buffer, lobby_over.data(), received_size);
	}
}

template <class T>
void NWMODULE<T>::RecvBattleThread()
{
	int received_size = 0;
	while (true)
	{
		received_size = recv(battle_socket, battle_over.data(), MAX_BUFFER_SIZE, 0);
		if (received_size == 0 || received_size == SOCKET_ERROR) {
			cout << "[Connection Closed]" << endl;
			return;
		}
		packet_drain(battle_buffer, battle_over.data(), received_size);
	}
}

template <class T>
NWMODULE<T>::NWMODULE(T& MainModule, int buffer_size, HANDLE iocp, int ev_send, int ev_lobby, int ev_battle) :
	iocp(iocp),
	lobby_socket(INVALID_SOCKET),
	battle_socket(INVALID_SOCKET),
	MainModule(MainModule),
	lobby_buffer(sizeof(PACKET_SIZE)),
	battle_buffer(sizeof(PACKET_SIZE)),
	lobby_over(0, buffer_size),
	battle_over(0, buffer_size),
	ev_send(ev_send),
	ev_battle(ev_battle),
	ev_lobby(ev_lobby)
{
	callbacks.reserve(CSSS_PACKET_COUNT);
	for (int i = 0; i < CSSS_PACKET_COUNT; ++i)
		callbacks.emplace_back([a = i](T&, packet_inheritance*) {printf("ENROLL PACKET TYPE %d\n", a); });
	InitWSA();
}

template <class T>
NWMODULE<T>::~NWMODULE()
{
	for (int i = 0; i < threads.size(); ++i)
		threads[i].join();
	WSACleanup();
}


template <class T>
bool NWMODULE<T>::connect_lobby()
{
	bool retval = false;
	retval = connect_server(lobby_socket, lobby_addr.c_str(), lobby_port);
	if (false == retval) return retval;

	if (INVALID_HANDLE_VALUE == iocp)
		threads.emplace_back(&NWMODULE<T>::RecvLobbyThread, this);
	else {
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(lobby_socket), iocp, iocp_key, 0);
		lobby_over.reset();
		int ret = WSARecv(lobby_socket, lobby_over.buffer(), 1, NULL,
			&lobby_recv_flag, lobby_over.overlapped(), NULL);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				error_display("WSARecv Error :", err_no);
		}
	}
	return retval;
}

template<class T>
void NWMODULE<T>::disconnect_lobby()
{
	process_disconnect(lobby_socket, lobby_buffer);
}

template <class T>
bool NWMODULE<T>::connect_battle()
{
	bool retval = connect_server(battle_socket, battle_addr.c_str(), battle_port);
	if (false == retval) return retval;

	if (INVALID_HANDLE_VALUE == iocp)
		threads.emplace_back(&NWMODULE<T>::RecvBattleThread, this);
	else {
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(battle_socket), iocp, iocp_key, 0);
		battle_over.reset();
		int ret = WSARecv(battle_socket, battle_over.buffer(), 1, NULL,
			&battle_recv_flag, battle_over.overlapped(), NULL);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				error_display("WSARecv Error :", err_no);
		}
	}

	return retval;
}


template<class T>
void NWMODULE<T>::disconnect_battle()
{
	process_disconnect(battle_socket, battle_buffer);
}

#ifndef NO_BASIC_FUNCTION
template <class T>
void NWMODULE<T>::request_login(const char* id)
{
	string s_id {id};
	cs_packet_request_login packet;
	packet.cdp.size = sizeof(cs_packet_request_login);
	packet.cdp.type = CS_PACKET_REQUEST_LOGIN;
	strcpy_s(packet.id, ID_LENGTH-1, s_id.c_str());
	send(lobby_socket, reinterpret_cast<const char*>(&packet), packet.cdp.size, 0);
}

template <class T>
void NWMODULE<T>::add_friend(const char* id)
{
	cs_packet_request_friend packet;
	packet.cdp.size = sizeof(cs_packet_request_friend);
	packet.cdp.type = CS_PACKET_REQUEST_FRIEND;
	strcpy_s(packet.id, ID_LENGTH-1, id);
	send(lobby_socket, reinterpret_cast<const char*>(&packet), packet.cdp.size, 0);
}

template <class T>
void NWMODULE<T>::accept_friend(const char* id)
{
	cs_packet_accept_friend packet;
	packet.cdp.size = sizeof(cs_packet_accept_friend);
	packet.cdp.type = CS_PACKET_ACCEPT_FRIEND;
	strcpy_s(packet.id, ID_LENGTH - 1, id);
	send(lobby_socket, reinterpret_cast<const char*>(&packet), packet.cdp.size, 0);
}

template <class T>
void NWMODULE<T>::match_enqueue()
{
	send_default_packet(CS_PACKET_MATCH_ENQUEUE);
}

template <class T>
void NWMODULE<T>::match_dequeue()
{
	send_default_packet(CS_PACKET_MATCH_DEQUEUE);
}
#endif

template <class T>
void NWMODULE<T>::enroll_callback(int packet_type, function<void(T&, packet_inheritance*)> callback)
{
	if (callback == nullptr) return;
	callbacks[packet_type] = callback;
}

template<class T>
void NWMODULE<T>::notify_lobby_recv(size_t length)
{
	if (length == 0 || length == SOCKET_ERROR)
		disconnect_lobby();
	else {
		packet_drain(lobby_buffer, lobby_over.data(), length);
		
		lobby_over.reset();
		int ret = WSARecv(lobby_socket, lobby_over.buffer(), 1, NULL,
			&lobby_recv_flag, lobby_over.overlapped(), NULL);
		//if (0 != ret) {
		//	int err_no = WSAGetLastError();
		//	if (WSA_IO_PENDING != err_no)
		//		error_display("WSARecv Error :", err_no);
		//}
	}
}

template<class T>
void NWMODULE<T>::notify_battle_recv(size_t length)
{
	if (length == 0 || length == SOCKET_ERROR)
		disconnect_battle();
	else {
		packet_drain(battle_buffer, battle_over.data(), length);

		battle_over.reset();
		int ret = WSARecv(battle_socket, battle_over.buffer(), 1, NULL,
			&battle_recv_flag, battle_over.overlapped(), NULL);
		//if (0 != ret) {
		//	int err_no = WSAGetLastError();
		//	if (WSA_IO_PENDING != err_no)
		//		error_display("WSARecv Error :", err_no);
		//}
	}
}

template<class T>
void NWMODULE<T>::update()
{
	//Lobby Update
	lobby_buffer.CompletetoProcess();
	char* packet_pos = lobby_buffer.p_packet.data;
	size_t packet_length = lobby_buffer.p_packet.len;
	size_t packet_size = 0;
	while (packet_length > 0)
	{
		const packet_inheritance* packet = reinterpret_cast<const packet_inheritance*>(packet_pos);
		packet_size = packet->size;
		process_packet(packet->type, packet_pos);
		packet_length -= packet_size;
		packet_pos += packet_size;
	}

	//Battle Update
	battle_buffer.CompletetoProcess();
	packet_pos = battle_buffer.p_packet.data;
	packet_length = battle_buffer.p_packet.len;
	packet_size = 0;
	while (packet_length > 0)
	{
		const packet_inheritance* packet = reinterpret_cast<const packet_inheritance*>(packet_pos);
		packet_size = packet->size;
		process_packet(packet->type, packet_pos);
		packet_length -= packet_size;
		packet_pos += packet_size;
	}
}