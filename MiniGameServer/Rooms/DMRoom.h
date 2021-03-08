#pragma once
#include <mutex>
#include <map>
#include <set>
#include <DirectXCollision.h>
#include <atomic>
#include "..\Common\PacketVector.h"

class DMRoom
{
public:
	DMRoom();
	virtual ~DMRoom();
	virtual void Init();
	virtual bool Regist();
	virtual void Disconnect();
	virtual void start();
	virtual bool update(float elapsedTime);

	PacketVector eventData; //전송될 이벤트 패킷(플레이어 힛, 리스폰 등)
	PacketVector infoData;  //전송될 위치정보 패킷
private:
	std::chrono::high_resolution_clock::time_point current_update_time;
	std::chrono::high_resolution_clock::time_point last_update_time;

	std::mutex packet_lock;
	PacketVector remainPackets;
	PacketVector processPackets;

	float delta_time;
	float left_time;
	
	void ProcessPackets();
	bool GameLogic();
	void SendGameState();
	void ProcessPacket();
};