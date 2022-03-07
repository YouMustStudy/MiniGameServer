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

/**
*@brief 방향 정보 정의.
*@author Gurnwoo Kim.
*/
struct MoveDirInfo
{
	UID uid;
	float x;
	float y;

	MoveDirInfo(UID uid, float x, float y) : uid(uid), x(x), y(y) {};
};

/**
*@brief 실제 게임이 이뤄지는 객체. 게임 종료 시 룸매니저로 회수 요청하는 로직으로 구성.
*@author Gurnwoo Kim.
*/
class DMRoom : public LockFreeQueue
{
	friend Character;

protected:
	/**
	*@brief 작업을 처리한다.
	*param[in] job 작업.
	*/
	virtual void ProcessJob(Job job) override;

private:
	//기본 방 운영 정보
	std::chrono::high_resolution_clock::time_point currentUpdateTime; ///< deltaTime 계산용
	std::chrono::high_resolution_clock::time_point lastUpdateTime;	  ///< deltaTime 계산용

	PacketVector eventData;                      ///< 전송될 이벤트 패킷(플레이어 힛, 리스폰 등)
	PacketVector infoData;                       ///< 전송될 위치정보 패킷
	bool         isEnd{ false };		         ///< 게임이 끝났는가?
	float        deltaTime{};		             ///< 매 틱 변한 시간
	float        leftTime{ DEFAULT_MATCH_TIME }; ///< 남은 게임 시간
	size_t       readyCount{ 0 };	             ///< 레디한 유저 수
	UID          serverID = 0;                   ///< 서버에서 관리하는 오브젝트 Id
	Collider     mapCollider                     ///< 낙사 판정용 충돌체
	{
		MAP_WIDTH,
		MAP_HEIGHT,
		{0, 0, 0},
		true 
	};   

	std::vector<Character> characterList{};	     ///< 플레이하는 '캐릭터' 컨테이너
	std::vector<User*>     userList{};		     ///< 플레이중인 '유저' 컨테이너
	Vector3d initialPos[ 5 ]                     ///< 캐릭터 시작위치
	{					 
		{-600.0f, -600.0f, 0.0f},
		{ 600.0f, -600.0f, 0.0f},
		{-600.0f,  600.0f, 0.0f},
		{ 600.0f,  600.0f, 0.0f}
	};

	std::mt19937_64                       randomEngine;               ///< 난수 생성기
	std::uniform_real_distribution<float> randomRange{ -1.0f, 1.0f }; ///< 난수 생성 범위

	/**
	*@brief 게임 로직을 수행한다.
	*/
	void GameLogic();

	/**
	*@brief 게임 상태를 유저에게 보낸다.
	*/
	void SendGameState();

	/**
	*@brief 게임이 종료되었는 지 확인한다.
	*/
	bool EndCheck();

	/**
	*@brief 모든 유저를 내보낸다.
	*/
	void QuitAllUser();

	/**
	*@brief 유저의 연결 종료를 처리한다.
	*@param[in] user 유저.
	*/
	void Disconnect( User* user );

	/**
	*@brief 게임 종료를 처리한다.
	*/
	void End();

	/**
	*@brief 게임을 업데이트한다.
	*/
	void Update();

	/**
	*@brief 공격을 처리한다.
	*@param[in] uid 유저 식별자.
	*/
	void ProcessAttack( UID uid );

	/**
	*@brief 움직임을 처리한다.
	*@param[in] info 움직임 정보.
	*/
	void ProcessMoveDir( MoveDirInfo* info );

	/**
	*@brief 준비를 처리한다.
	*@param[in] uid 유저 식별자.
	*/
	void ProcessReady(UID uid);

	/**
	*@brief 남은 시간을 확인한다.
	*/
	void UpdateLeftTime();

	/**
	*@brief 위치를 갱신한다.
	*/
	void UpdatePosition();

	/**
	*@brief 충돌 처리를 수행한다.
	*/
	void UpdateCollider();

	/**
	*@brief 충돌 여부를 반환한다.
	*@param[in] a 충돌을 검사할 객체.
	*@param[in] b 충돌을 검사할 객체.
	*@return 충돌 여부.
	*/
	bool CheckCollider(const Collider& a, const Collider& b);

public:
	/**
	*@brief 기본 생성자.
	*/
	DMRoom();

	/**
	*@brief 기본 소멸자.
	*/
	virtual ~DMRoom();

	/**
	*@brief LFQ 타입을 설정한다.
	*@param[in] queType 큐 타입.
	*/
	void SetQueueType( QueueType queType );

	/**
	*@brief 룸을 초기화한다. 
	*@details Not Thread-Safe, RoomManager에서만 호출할 것.
	*/
	void Init();

	/**
	*@brief 유저를 등록한다.
	*@details Not Thread-Safe, RoomManager에서만 호출할 것.
	*/
	void Regist( std::vector<User*> users );

	/**
	*@brief 게임의 종료 여부를 반환한다.
	*@return 게임의 종료 여부.
	*/
	bool IsEnd() { return isEnd; };
};