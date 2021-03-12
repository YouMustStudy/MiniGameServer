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
	bool GameLogic();
	void SendGameState();

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
	float leftTime{};		//���� ���� �ð�

	//���� ���� ó��
	void ProcessAttack(UID uid);
	void ProcessMoveDir(MoveDirInfo* info);

	void UpdatePosition();
	void KnockBack(Character& character);
	void UpdatePos(Character& character);
	void UpdateCollider();
	bool CheckCollider(Collider& a, Collider& b);

	std::vector<Character> characterList{};	//�÷����ϴ� 'ĳ����' �����̳�
	std::vector<User*> userList{};			//�÷������� '����' �����̳�

	Collider mapCollider{ 0, 0, MAP_WIDTH, MAP_HEIGHT, {0, 0, 0} };
};