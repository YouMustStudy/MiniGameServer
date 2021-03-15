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
constexpr float MAP_WIDTH = 1000.0f;
constexpr float MAP_HEIGHT = 1000.0f;
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
	static constexpr long long UPDATE_INTERVAL = 20;					//������Ʈ ����
	std::chrono::high_resolution_clock::time_point currentUpdateTime;	//deltaTime ������
	std::chrono::high_resolution_clock::time_point lastUpdateTime;		//��

	PacketVector eventData; //���۵� �̺�Ʈ ��Ŷ(�÷��̾� ��, ������ ��)
	PacketVector infoData;  //���۵� ��ġ���� ��Ŷ
	bool isEnd{false};		//������ �����°�?
	float deltaTime{};		//�� ƽ ���� �ð�
	static constexpr float DEFAULT_MATCH_TIME = 180.0f;
	float leftTime{ DEFAULT_MATCH_TIME };		//���� ���� �ð�

	
	

	//���� ���� ó��
	void ProcessAttack(UID uid);
	void ProcessMoveDir(MoveDirInfo* info);

	void UpdateLeftTime();
	void UpdatePosition();
	void UpdateCollider();
	bool CheckCollider(const Collider& a, const Collider& b);

	std::vector<Character> characterList{};	//�÷����ϴ� 'ĳ����' �����̳�
	std::vector<User*> userList{};			//�÷������� '����' �����̳�

	Collider mapCollider{MAP_WIDTH, MAP_HEIGHT, {0, 0, 0} };	//���� �浹��ü, ���� ���� �� ���
	std::mt19937_64 randomEngine;
	std::uniform_real_distribution<float> randomRange{-1.0f, 1.0f};
};