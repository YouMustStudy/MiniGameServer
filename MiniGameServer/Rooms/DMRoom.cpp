#include "DMRoom.h"
#include "..\Common\Client.h"
#include "..\Utills\Logger.h"

DMRoom::DMRoom()
{
}

DMRoom::~DMRoom()
{
}

void DMRoom::Init()
{
}

bool DMRoom::Regist()
{
	return true;
}

void DMRoom::Disconnect()
{
}

void DMRoom::Start()
{
	last_update_time = std::chrono::high_resolution_clock::now();
}

bool DMRoom::Update()
{
	current_update_time = std::chrono::high_resolution_clock::now();
	delta_time = std::chrono::duration<float>(current_update_time - last_update_time).count();
	last_update_time = current_update_time;
	ParsePackets();
    GameLogic();
	SendGameState();
    return false;
}

void DMRoom::ParsePackets()
{
	processPackets.Clear();
	processPackets.EmplaceBack(remainPackets.data, remainPackets.len);
	remainPackets.Clear();

	char* packet_pos = processPackets.data;
	PACKET_SIZE packet_size = 0;
	size_t packet_length = processPackets.len;
	while (packet_length > 0)
	{
		DEFAULT_PACKET* dp = reinterpret_cast<DEFAULT_PACKET*>(packet_pos);
		packet_size = dp->size;
		ProcessPacket(packet_pos);
		packet_length -= packet_size;
		packet_pos += packet_size;
	}
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

void DMRoom::ProcessPacket(const char* buffer)
{
	if (nullptr == buffer) return;
	const DEFAULT_PACKET* dp = reinterpret_cast<const DEFAULT_PACKET*>(buffer);
	switch (dp->type)
	{
	default:
		Logger::Log("NO TYPE PACKET FOR ROOM");
		break;
	}
}
