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
*@brief ���� ���� ����.
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
*@brief ���� ������ �̷����� ��ü. ���� ���� �� ��Ŵ����� ȸ�� ��û�ϴ� �������� ����.
*@author Gurnwoo Kim.
*/
class DMRoom : public LockFreeQueue
{
	friend Character;

protected:
	/**
	*@brief �۾��� ó���Ѵ�.
	*param[in] job �۾�.
	*/
	virtual void ProcessJob(Job job) override;

private:
	//�⺻ �� � ����
	std::chrono::high_resolution_clock::time_point currentUpdateTime; ///< deltaTime ����
	std::chrono::high_resolution_clock::time_point lastUpdateTime;	  ///< deltaTime ����

	PacketVector eventData;                      ///< ���۵� �̺�Ʈ ��Ŷ(�÷��̾� ��, ������ ��)
	PacketVector infoData;                       ///< ���۵� ��ġ���� ��Ŷ
	bool         isEnd{ false };		         ///< ������ �����°�?
	float        deltaTime{};		             ///< �� ƽ ���� �ð�
	float        leftTime{ DEFAULT_MATCH_TIME }; ///< ���� ���� �ð�
	size_t       readyCount{ 0 };	             ///< ������ ���� ��
	UID          serverID = 0;                   ///< �������� �����ϴ� ������Ʈ Id
	Collider     mapCollider                     ///< ���� ������ �浹ü
	{
		MAP_WIDTH,
		MAP_HEIGHT,
		{0, 0, 0},
		true 
	};   

	std::vector<Character> characterList{};	     ///< �÷����ϴ� 'ĳ����' �����̳�
	std::vector<User*>     userList{};		     ///< �÷������� '����' �����̳�
	Vector3d initialPos[ 5 ]                     ///< ĳ���� ������ġ
	{					 
		{-600.0f, -600.0f, 0.0f},
		{ 600.0f, -600.0f, 0.0f},
		{-600.0f,  600.0f, 0.0f},
		{ 600.0f,  600.0f, 0.0f}
	};

	std::mt19937_64                       randomEngine;               ///< ���� ������
	std::uniform_real_distribution<float> randomRange{ -1.0f, 1.0f }; ///< ���� ���� ����

	/**
	*@brief ���� ������ �����Ѵ�.
	*/
	void GameLogic();

	/**
	*@brief ���� ���¸� �������� ������.
	*/
	void SendGameState();

	/**
	*@brief ������ ����Ǿ��� �� Ȯ���Ѵ�.
	*/
	bool EndCheck();

	/**
	*@brief ��� ������ ��������.
	*/
	void QuitAllUser();

	/**
	*@brief ������ ���� ���Ḧ ó���Ѵ�.
	*@param[in] user ����.
	*/
	void Disconnect( User* user );

	/**
	*@brief ���� ���Ḧ ó���Ѵ�.
	*/
	void End();

	/**
	*@brief ������ ������Ʈ�Ѵ�.
	*/
	void Update();

	/**
	*@brief ������ ó���Ѵ�.
	*@param[in] uid ���� �ĺ���.
	*/
	void ProcessAttack( UID uid );

	/**
	*@brief �������� ó���Ѵ�.
	*@param[in] info ������ ����.
	*/
	void ProcessMoveDir( MoveDirInfo* info );

	/**
	*@brief �غ� ó���Ѵ�.
	*@param[in] uid ���� �ĺ���.
	*/
	void ProcessReady(UID uid);

	/**
	*@brief ���� �ð��� Ȯ���Ѵ�.
	*/
	void UpdateLeftTime();

	/**
	*@brief ��ġ�� �����Ѵ�.
	*/
	void UpdatePosition();

	/**
	*@brief �浹 ó���� �����Ѵ�.
	*/
	void UpdateCollider();

	/**
	*@brief �浹 ���θ� ��ȯ�Ѵ�.
	*@param[in] a �浹�� �˻��� ��ü.
	*@param[in] b �浹�� �˻��� ��ü.
	*@return �浹 ����.
	*/
	bool CheckCollider(const Collider& a, const Collider& b);

public:
	/**
	*@brief �⺻ ������.
	*/
	DMRoom();

	/**
	*@brief �⺻ �Ҹ���.
	*/
	virtual ~DMRoom();

	/**
	*@brief LFQ Ÿ���� �����Ѵ�.
	*@param[in] queType ť Ÿ��.
	*/
	void SetQueueType( QueueType queType );

	/**
	*@brief ���� �ʱ�ȭ�Ѵ�. 
	*@details Not Thread-Safe, RoomManager������ ȣ���� ��.
	*/
	void Init();

	/**
	*@brief ������ ����Ѵ�.
	*@details Not Thread-Safe, RoomManager������ ȣ���� ��.
	*/
	void Regist( std::vector<User*> users );

	/**
	*@brief ������ ���� ���θ� ��ȯ�Ѵ�.
	*@return ������ ���� ����.
	*/
	bool IsEnd() { return isEnd; };
};