#pragma once
#include <mutex>
#include <map>
#include <set>
#include <DirectXCollision.h>
#include <atomic>
#include <vector>
#include "..\Common\PacketVector.h"
#include "..\LockFreeQueue.h"

class User;
/**
룸
실제 게임이 이뤄지는 객체
룸 포인터의 수정은 해당 객체 내에서만 발생한다.
*/
class DMRoom : public LockFreeQueue
{
public:
	DMRoom();
	virtual ~DMRoom();

	void SetQueueType(QueueType queType);

	void Init();								//Not Thread-Safe, RoomManager에서만 호출할 것.
	void Regist(std::vector<User*> users);	//Not Thread-Safe, RoomManager에서만 호출할 것.
	bool IsEnd() { return isEnd; };


protected:
	virtual void ProcessJob(Job job) override;

private:
	static constexpr long long UPDATE_INTERVAL = 20;

	std::chrono::high_resolution_clock::time_point currentUpdateTime;
	std::chrono::high_resolution_clock::time_point lastUpdateTime;

	PacketVector eventData; //전송될 이벤트 패킷(플레이어 힛, 리스폰 등)
	PacketVector infoData;  //전송될 위치정보 패킷
	bool isEnd{false};

	float deltaTime{};
	float left_time{1.0f};

	bool GameLogic();
	void SendGameState();

	void Disconnect(User* user);
	void End();
	void Update();
};