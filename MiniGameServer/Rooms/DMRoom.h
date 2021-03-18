#pragma once
#include <mutex>
#include <map>
#include <set>
#include <DirectXCollision.h>
#include <atomic>
#include <vector>
#include <random>
#include "..\Common\PacketVector.h"
#include "..\LockFreeQueue.h"
#include "..\Character.h"
#include "..\protocol.h"
#include "..\ServerConfig.h"

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

GameLogic (종료체크도 시행)
SendGameState (게임 상태 전송)

게임 종료 시 룸매니저로 회수 요청하는 로직으로 구성
*/

class DMRoom : public LockFreeQueue
{
	friend Character;

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
	void GameLogic();
	void SendGameState();
	bool EndCheck();
	void QuitAllUser();

	void Disconnect(User* user);
	void End();
	void Update();

	//기본 방 운영 정보
	std::chrono::high_resolution_clock::time_point currentUpdateTime;	//deltaTime 측정용
	std::chrono::high_resolution_clock::time_point lastUpdateTime;		//상동

	PacketVector eventData; //전송될 이벤트 패킷(플레이어 힛, 리스폰 등)
	PacketVector infoData;  //전송될 위치정보 패킷
	bool isEnd{false};		//게임이 끝났는가?
	float deltaTime{};		//매 틱 변한 시간
	float leftTime{ DEFAULT_MATCH_TIME };		//남은 게임 시간
	size_t readyCount{ 0 };	//레디한 유저 수

	//실제 게임 처리
	//패킷 처리부
	void ProcessAttack(UID uid);
	void ProcessMoveDir(MoveDirInfo* info);
	void ProcessReady(UID uid);

	void UpdateLeftTime();	//남은 시간 체크 - 1초단위로 클라에게 중계
	void UpdatePosition();	//위치 업데이트
	void UpdateCollider();	//충돌 처리
	bool CheckCollider(const Collider& a, const Collider& b);

	std::vector<Character> characterList{};	//플레이하는 '캐릭터' 컨테이너
	std::vector<User*> userList{};			//플레이중인 '유저' 컨테이너
	Vector3d initialPos[5]{					//캐릭터 시작위치
		{-800.0f, -800.0f, 0.0f},
		{800.0f, -800.0f, 0.0f},
		{-800.0f, 800.0f, 0.0f},
		{800.0f, 800.0f, 0.0f},
	};

	Collider mapCollider{MAP_WIDTH, MAP_HEIGHT, {0, 0, 0} };	//맵의 충돌객체, 낙사 판정 시 사용
	std::mt19937_64 randomEngine;
	std::uniform_real_distribution<float> randomRange{-1.0f, 1.0f};


	// 폭탄을 포함해서 증가하는 id
	UID serverID = 0;
};