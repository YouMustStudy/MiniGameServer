#pragma once
#include <mutex>
#include <map>
#include <set>
#include <DirectXCollision.h>
#include <atomic>
#include <vector>
#include "..\Common\PacketVector.h"
#include "..\LockFreeQueue.h"

class Client;
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
	void Regist(std::vector<Client*> clients);	//Not Thread-Safe, RoomManager������ ȣ���� ��.
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

	float delta_time{};
	float left_time{};

	bool GameLogic();
	void SendGameState();

	void Disconnect();
	void End();
	void Update();
};