#pragma once
#include <mutex>
#include <map>
#include <set>
#include <DirectXCollision.h>
#include <atomic>
#include <vector>
#include "..\Common\PacketVector.h"
#include "..\LockFreeQueue.h"
#include "..\Character.h"
#include "..\protocol.h"

class User;

struct MoveDirInfo
{
	UID uid;
	float x;
	float y;

	MoveDirInfo(UID uid, float x, float y) : uid(uid), x(x), y(y) {};
};

/**
룸
실제 게임이 이뤄지는 객체

*/
class DMRoom : public LockFreeQueue
{
public:
	DMRoom();
	virtual ~DMRoom();

	void SetQueueType(QueueType queType);

	void Init();							//Not Thread-Safe, RoomManager에서만 호출할 것.
	void Regist(std::vector<User*> users);	//Not Thread-Safe, RoomManager에서만 호출할 것.
	bool IsEnd() { return isEnd; };


protected:
	virtual void ProcessJob(Job job) override;

private:
	bool GameLogic();
	void SendGameState();

	void Disconnect(User* user);
	void End();
	void Update();

	//기본 방 운영 정보
	static constexpr long long UPDATE_INTERVAL = 20;
	std::chrono::high_resolution_clock::time_point currentUpdateTime;
	std::chrono::high_resolution_clock::time_point lastUpdateTime;

	PacketVector eventData; //전송될 이벤트 패킷(플레이어 힛, 리스폰 등)
	PacketVector infoData;  //전송될 위치정보 패킷
	bool isEnd{false};
	float deltaTime{};
	//float left_time{1.0f};

	//실제 게임 처리
	void ProcessAttack(UID uid);
	void ProcessMoveDir(MoveDirInfo* info);

	void UpdatePosition();
	void KnockBack(Character& character);
	void UpdatePos(Character& character);
	void UpdateCollider();
	bool CheckCollider(Collider& a, Collider& b);
	
	std::vector<Character> characterList;
	std::vector<User*> userList;
};