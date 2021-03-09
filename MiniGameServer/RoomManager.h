#pragma once
#include <array>
#include <stack>
#include <chrono>
#include "Common/Client.h"
#include "Rooms/DMRoom.h"
#include "LockFreeQueue.h"


enum RoomManagerJobType
{
	RMGR_CREATE,
	RMGR_DESTROY
};

struct CreateRoomInfo
{
	CreateRoomInfo(const std::vector<Client*>& userList) : users(userList) {};

	std::vector<Client*> users;
};

/**
��Ŵ���
��(��ġ)�� ����, ������ ���þƼ� �ϴ� ��ü
*/
class RoomManager : public LockFreeQueue
{
public:
	virtual ~RoomManager() {};
	static RoomManager& Instance();
	DMRoom* GetRoom(size_t idx);

protected:
	virtual void ProcessJob(Job job) override;

private:
	RoomManager();

	void ProcessCreateRoom(CreateRoomInfo* info);
	void ProcessDestroyRoom(size_t idx);

	static constexpr size_t MAX_ROOM_SIZE = 200;
	std::array<DMRoom, MAX_ROOM_SIZE> roomList{};
	std::stack<size_t> indexPool;
};