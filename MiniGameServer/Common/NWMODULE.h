#pragma once
#pragma comment(lib, "ws2_32")

#include <WS2tcpip.h>
#include <thread>
#include <functional>

#include <unordered_map>
#include <vector>
#include <mutex>

#include "PACKET_BUFFER.h"
#include "OVER_EX.h"
#include "CSOCKADDR_IN.h"
#include "..\..\Streaming\Streaming_Server\Streaming_Server\packet_struct.h"

using namespace std;

#define NO_BASIC_FUNCTION

/**
@brief class for communicate battle and lobby server.
@author Gurnwoo Kim
*/
template <class T>
class NWMODULE
{
public:
	NWMODULE(T& MainModule, int buffer_size = 512, HANDLE iocp = INVALID_HANDLE_VALUE, int ev_send = 0, int ev_lobby = 0, int ev_battle = 0);


	~NWMODULE();

	/**
	@brief Init Config from ini.
	*/
	void InitConfig();

	/**
	@brief enroll packets callback.
	@param packet_type target packet.
	@param callback when packet_type arrived, this callback will execute.
	*/
	void enroll_callback(int packet_type, function<void(T&, packet_inheritance*)> callback);


	/**
	@brief notify recv from lobby. - use with iocp env.
	@param length received data length.
	*/
	void notify_lobby_recv(size_t length);

	/**
	@brief notify recv from battle. - use with iocp env.
	@param length received data length.
	*/
	void notify_battle_recv(size_t length);

	/**
	@brief update packets to client. call before main logic.
	*/
	void update();

	//Connect Server Funcs

	/**
	@brief connect to lobby.
	@param iocp_key key for iocp. if there's no iocp handle, no need.
	@return bool that connection is estalished or not.
	*/
	bool connect_lobby();

	/**
	@brief disconnect from lobby.
	*/
	void disconnect_lobby();

	/**
	@brief connect to battle.
	@param iocp_key key for iocp. if there's no iocp handle, no need.
	@return bool that connection is estalished or not.
	*/
	bool connect_battle();

	/**
	@brief disconnect from battle.
	*/
	void disconnect_battle();

	/**
	@brief send packet to lobby or battle.
	@param packet.
	*/
	void send_packet(packet_inheritance* packet);

	/**
	@brief all tasks kill
	*/
	void destroy();


#ifndef NO_BASIC_FUNCTION
	/**
	@brief request login to lobby.
	@param id login id. max 10 length.
	*/
	void request_login(const char* id);

	/**
	@brief request add friend to lobby.
	@param id : login id. max 10.
	*/
	void add_friend(const char* id);

	/**
	@brief answer accepting friend request to lobby.
	@param id : login id. max 10.
	*/
	void accept_friend(const char* id);

	/**
	@brief reqeust client enqueue match_make pool to lobby.
	*/
	void match_enqueue();

	/**
	@brief reqeust client dequeue match_make pool to lobby.
	*/
	void match_dequeue();
	
	
#endif

	/**
	@brief Set iocp envionment.
	*/
	void set_iocp(HANDLE iocp, int iocp_key ,int ev_send, int ev_lobby, int ev_battle)
	{
		this->iocp = iocp;
		this->iocp_key = iocp_key;
		this->ev_send = ev_send;
		this->ev_lobby = ev_lobby;
		this->ev_battle = ev_battle;
		lobby_over.set_event(ev_lobby);
		battle_over.set_event(ev_battle);
	}



private:
	T& MainModule;									///< Main class need to attach NW MODULE.
	HANDLE iocp;									///< Handle for IOCP.
	SOCKET lobby_socket;							///< Socket for lobby.
	SOCKET battle_socket;							///< Socket for battle.
	vector<thread> threads;							///< Thread list operated in NW MODULE.
	vector<function<void(T&, packet_inheritance*)>> callbacks;			///< Callbacks when packet arrived.

	std::wstring config_path{ L".\\CLIENT_CONFIG.ini" };

	std::wstring lobby_addr;
	short lobby_port;
	std::wstring battle_addr;
	short battle_port;

	OverEx lobby_over;								///< EXPENDED OVERLAPPED Structure for lobby.
	PACKET_BUFFER lobby_buffer;						///< Buffer for complete packet.
	OverEx battle_over;							///< EXPENDED OVERLAPPED Structure for battle.
	PACKET_BUFFER battle_buffer;					///< Buffer for complete packet.
	DWORD lobby_recv_flag{ 0 };
	DWORD battle_recv_flag{ 0 };

	int iocp_key;
	int ev_send;
	int ev_lobby;
	int ev_battle;
private:
	

	void error_display(const char* msg, int err_no);

    /**
    @brief Initializing WSA environment.
    */
	void InitWSA();

	/**
	@brief if there's no iocp, make recv thread for continous recv from lobby.
	*/
	void RecvLobbyThread();

	/**
	@brief if there's no iocp, make recv thread for continous recv from battle.
	*/
	void RecvBattleThread();

	/**
	@brief send default type packet to lobby.
	@param type packet type.
	*/
	void send_default_packet(int type);

	/**
	@brief connect to server.
	@param socket return with new socket.
	@param address address to connecting server. ex) 192.168.0.1
	@param port server port with big-endian.
	@return true when success or not.
	*/
	bool connect_server(SOCKET& socket, const wchar_t* address, const short port);

	/**
	@brief read and make complete packet from buffer after recv and save to packet_buffer.
	@param packet_buffer complete packet will save to this.
	@param buffer buffer that need to drain.
	@param len received data length.
	*/
	void packet_drain(PACKET_BUFFER& packet_buffer, char* buffer, size_t len);

	/**
	@brief process disconnect to socket and clear packet_buffer.
	@param socket socket that want to disconnect.
	@param buffer packet_buffer that want to disconnect.
	*/
	void process_disconnect(SOCKET& socket, PACKET_BUFFER& buffer);

	/**
	@brief process packet and call appropriate callback.
	@param type packets type.
	@param buffer packets data.
	*/
	void process_packet(int packet_type, void* buffer);

	/**
	@brief Generate default ini file.
	*/
	void gen_default_config();
};



