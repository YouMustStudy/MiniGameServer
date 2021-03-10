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
��
���� ������ �̷����� ��ü
�� �������� ������ �ش� ��ü �������� �߻��Ѵ�.
*/
class DMRoom : public LockFreeQueue
{
public:
	DMRoom();
	virtual ~DMRoom();

	void SetQueueType(QueueType queType);

	void Init();								//Not Thread-Safe, RoomManager������ ȣ���� ��.
	void Regist(std::vector<User*> users);	//Not Thread-Safe, RoomManager������ ȣ���� ��.
	bool IsEnd() { return isEnd; };


protected:
	virtual void ProcessJob(Job job) override;

private:
	static constexpr long long UPDATE_INTERVAL = 20;

	std::chrono::high_resolution_clock::time_point currentUpdateTime;
	std::chrono::high_resolution_clock::time_point lastUpdateTime;

	PacketVector eventData; //���۵� �̺�Ʈ ��Ŷ(�÷��̾� ��, ������ ��)
	PacketVector infoData;  //���۵� ��ġ���� ��Ŷ
	bool isEnd{false};

	float deltaTime{};
	float left_time{1.0f};

	bool GameLogic();
	void SendGameState();

	void Disconnect(User* user);
	void End();
	void Update();
};