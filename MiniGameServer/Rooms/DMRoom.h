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
��
���� ������ �̷����� ��ü

GameLogic (����üũ�� ����)
SendGameState (���� ���� ����)

���� ���� �� ��Ŵ����� ȸ�� ��û�ϴ� �������� ����
*/

class DMRoom : public LockFreeQueue
{
	friend Character;

public:
	DMRoom();
	virtual ~DMRoom();

	void SetQueueType(QueueType queType);

	void Init();							//Not Thread-Safe, RoomManager������ ȣ���� ��.
	void Regist(std::vector<User*> users);	//Not Thread-Safe, RoomManager������ ȣ���� ��.
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

	//�⺻ �� � ����
	std::chrono::high_resolution_clock::time_point currentUpdateTime;	//deltaTime ������
	std::chrono::high_resolution_clock::time_point lastUpdateTime;		//��

	PacketVector eventData; //���۵� �̺�Ʈ ��Ŷ(�÷��̾� ��, ������ ��)
	PacketVector infoData;  //���۵� ��ġ���� ��Ŷ
	bool isEnd{false};		//������ �����°�?
	float deltaTime{};		//�� ƽ ���� �ð�
	float leftTime{ DEFAULT_MATCH_TIME };		//���� ���� �ð�
	size_t readyCount{ 0 };	//������ ���� ��

	//���� ���� ó��
	//��Ŷ ó����
	void ProcessAttack(UID uid);
	void ProcessMoveDir(MoveDirInfo* info);
	void ProcessReady(UID uid);

	void UpdateLeftTime();	//���� �ð� üũ - 1�ʴ����� Ŭ�󿡰� �߰�
	void UpdatePosition();	//��ġ ������Ʈ
	void UpdateCollider();	//�浹 ó��
	bool CheckCollider(const Collider& a, const Collider& b);

	std::vector<Character> characterList{};	//�÷����ϴ� 'ĳ����' �����̳�
	std::vector<User*> userList{};			//�÷������� '����' �����̳�
	Vector3d initialPos[5]{					//ĳ���� ������ġ
		{-800.0f, -800.0f, 0.0f},
		{800.0f, -800.0f, 0.0f},
		{-800.0f, 800.0f, 0.0f},
		{800.0f, 800.0f, 0.0f},
	};

	Collider mapCollider{MAP_WIDTH, MAP_HEIGHT, {0, 0, 0} };	//���� �浹��ü, ���� ���� �� ���
	std::mt19937_64 randomEngine;
	std::uniform_real_distribution<float> randomRange{-1.0f, 1.0f};


	// ��ź�� �����ؼ� �����ϴ� id
	UID serverID = 0;
};