#include "DMRoom.h"
#include "..\Common\User.h"
#include "..\Utills\Logger.h"
#include "..\RoomManager.h"
#include "..\UserManager.h"
#include "..\MiniGameServer.h"


DMRoom::DMRoom()
{
	std::random_device rd;
	randomEngine.seed(rd());
}

DMRoom::~DMRoom()
{
}

void DMRoom::Init()
{
	characterList.clear();
	userList.clear();
	eventData.Clear();
	infoData.Clear();
	leftTime = DEFAULT_MATCH_TIME;
	readyCount = 0;
	serverID = 0;
}

void DMRoom::ProcessJob(Job job)
{
	switch (job.first)
	{
	case CS_UPDATE:
		Update();
		break;

	case CS_LEAVEROOM:
		Disconnect(reinterpret_cast<User*>(job.second));
		break;

	case CS_ATTACK:
		//Logger::Log("���� ���� ���� " + std::to_string(reinterpret_cast<UID>(job.second)));
		ProcessAttack(reinterpret_cast<UID>(job.second));
		break;

	case CS_MOVEDIR:
		//Logger::Log("���� �𷺼� ���� " + std::to_string(reinterpret_cast<MoveDirInfo*>(job.second)->uid));
		ProcessMoveDir(reinterpret_cast<MoveDirInfo*>(job.second));
		break;

	case CS_READY:
		ProcessReady(reinterpret_cast<UID>(job.second));
		break;

	default:
		Logger::Log("ó������ ���� �� �� �߰�");
		break;
	}
}

void DMRoom::ProcessAttack(UID uid)
{
	if (EState::IDLE == characterList[uid]._playerInfo.curState
		|| EState::MOVE == characterList[uid]._playerInfo.curState)
	{
		// ������ ���󰡴��� ��������� �ʴ°�?
		// �뽬 ��Ŀ
		characterList[uid]._hitColl._attackedPos = Vector3d(
			characterList[uid]._playerInfo.pos.x + characterList[uid]._playerInfo.dir.x * DASH_WEIGHT * characterList[uid]._playerInfo.moveSpeed ,
			characterList[uid]._playerInfo.pos.y + characterList[uid]._playerInfo.dir.y * DASH_WEIGHT * characterList[uid]._playerInfo.moveSpeed ,
			characterList[uid]._playerInfo.pos.z);
		characterList[uid]._hitColl._bAttacked = true;

		// �����ϸ� �ϴ� ���ڸ�
		characterList[uid]._playerInfo.dir.x = 0;
		characterList[uid]._playerInfo.dir.y = 0;

		characterList[uid]._playerInfo.curState = EState::ATTACK_READY;
		characterList[uid]._playerInfo.animTime = 0.0f;

		//������Ŷ �߰�
		SC_PACKET_ATTACK atkPacket{ uid };
		eventData.EmplaceBack(&atkPacket, atkPacket.size);
	}
}

void DMRoom::ProcessMoveDir(MoveDirInfo* info)
{
	if (nullptr == info) return;
	if (characterList[info->uid]._playerInfo.curState == EState::ATTACK_READY ||
		characterList[info->uid]._hitColl._bAttacked == true) return; // ���û����̰ų�, �ǰ����̸� Ŭ��κ��� ��Ʈ�ѷ��� �ȹ���

	Vector3d newDir{ info->x, info->y, 0.0f };
	characterList[info->uid]._playerInfo.dir = newDir.normalize();

	delete info;
}

void DMRoom::ProcessReady(UID uid)
{
	if (false == characterList[uid]._playerInfo.isReady)
	{
		characterList[uid]._playerInfo.isReady = true;
		if (++readyCount == characterList.size())
		{
			//���� ������Ʈ ���� �߰�
			//���� �������� ���� ���ᰡ �߻��ϸ�?
			//��Ŀ�� �ʿ����� ���� ó�� �ʿ�
			Logger::Log("��� ���� �غ� �Ϸ�, ���� ����");
			lastUpdateTime = std::chrono::high_resolution_clock::now();
			MiniGameServer::Instance().AddEvent(queueType.second, EV_UPDATE, lastUpdateTime);
		};
	}
}

void DMRoom::UpdateLeftTime()
{
	float oldTime = leftTime;
	leftTime -= deltaTime;
	//1�ʴ����� Ŭ��/���� �� ���� �ð� ����ȭ
	if (1.0f <= oldTime)
	{
		TIME_TYPE oldSecTime = static_cast<TIME_TYPE>(oldTime);
		TIME_TYPE curSecTime = static_cast<TIME_TYPE>(leftTime);
		if (curSecTime != oldSecTime)
		{
			SC_PACKET_TIME timePacket{ curSecTime };
			eventData.EmplaceBack(&timePacket, timePacket.size);

			if (curSecTime % SUPER_BOMB_SPAWN_TIME == 0) // 4�� ��ź ��ȯ
			{
				for (int i = 0; i < 4; i++)
				{
					Logger::Log("��ź����");
					characterList.emplace_back((UID)serverID, this);
					characterList[serverID]._playerInfo.pos = initialPos[ i ];
					characterList[serverID].SetAbility(4);
					characterList[serverID]._playerInfo.isBomb = true;

					SC_PACKET_SPAWN_BOMB spawnBombPacket{ serverID,initialPos[i].x, initialPos[i].y };
					eventData.EmplaceBack(&spawnBombPacket, spawnBombPacket.size);

					serverID++;
				}
			}
			else if (curSecTime % BOMB_SPAWN_TIME == 0) // ��ź ��ȯ
			{
				Logger::Log("��ź����");
				characterList.emplace_back((UID)serverID, this);
				characterList[serverID]._playerInfo.pos = Vector3d{ 0,0,0 };
				characterList[serverID].SetAbility(4);
				characterList[serverID]._playerInfo.isBomb = true;

				SC_PACKET_SPAWN_BOMB spawnBombPacket{ serverID, 0, 0 };
				eventData.EmplaceBack(&spawnBombPacket, spawnBombPacket.size);

				serverID++;
			}

		}
	}
}

void DMRoom::UpdatePosition()
{
	for (auto& character : characterList)
		character.Update(deltaTime);
}

void DMRoom::UpdateCollider()
{
	// ���� ���� üũ 
	for (auto& chA : characterList) // �����ϴ� �÷��̾�
	{
		if (chA.GetAttackCollider()._enabled == false) continue; // �����ݶ��̴� Ȱ��ȭ X -> return
		for (auto& chB : characterList)	// �´� �÷��̾�
		{
			if (chA == chB) continue;	// �� �ڽ��� ���� ���Ѵ�.
			if (true == chB.IsInvincible()) continue;
			if (chA._playerInfo.isBomb == true && chB._playerInfo.isBomb == true) continue; // �� �� ��ź�̸� �浹 üũ ���� 
			if (chB._hitColl._enabled == false) continue; // �´� ĳ������ hitColl�� Ȱ��ȭ �ȵǸ� �浹 �н�
			if (true == CheckCollider(chA.GetAttackCollider(), chB.GetHitCollider())) // AttackColl, HitColl �浹 üũ
			{
				/* �ǰ�ü�� �ݶ��̴��� �ǰݴ��ѻ��·� �ٲٰ�, �з��� ��ġ�� �ο��Ѵ�. */
				chB._playerInfo.curState = EState::IDLE;		//���� ���� -> �̶� ������Ŷ���� �ٷ� �ݰ� ����
				chB.GetDamage(chA.id);

				/* �ǰ�ü�� �з����� ���� ���ϱ� */
				Vector3d disVec = chB._playerInfo.pos - chA._playerInfo.pos;

				//��ġ�� �����ϴٸ� ������ ��ġ�� Ƣ���� ���� ��������.
				while (true == disVec.isZero())
				{
					disVec.x = randomRange(randomEngine);
					disVec.y = randomRange(randomEngine);
				}
				disVec = disVec.normalize();

				//�˹� ��ġ �ο�
				chB.GetHitCollider()._bAttacked = true;

				//���ù���
				chB.GetHitCollider()._attackedPos = Vector3d(
					chB._playerInfo.pos.x + (chB._playerInfo.knockbackWeight * (chB._playerInfo.hitPoint) * chA._playerInfo.attackPower * disVec.x),
					chB._playerInfo.pos.y + (chB._playerInfo.knockbackWeight * (chB._playerInfo.hitPoint) * chA._playerInfo.attackPower * disVec.y),
					chB._playerInfo.pos.z
				);

				// ����Ʈ ��ȯ ��Ŷ �߰�
				SC_PACKET_SPAWN_EFFECT effectPacket{ 0, (int)EObjectType::HitEffect, chB._playerInfo.pos.x, chB._playerInfo.pos.y, chB._playerInfo.pos.z };
				eventData.EmplaceBack(&effectPacket, effectPacket.size);
			}
		}
		chA.GetAttackCollider()._enabled = false;
	}

	for (auto& ch : characterList) // �����ϴ� �÷��̾�
	{
		if (EState::FALL == ch.GetCurState()
			|| EState::DIE == ch.GetCurState())
			continue;

		//�� ������ ����� ���ó��
		if (false == CheckCollider(mapCollider, ch.GetHitCollider()))
		{
			ch._playerInfo.curState = EState::FALL;
			ch.GetAttackCollider()._enabled = false;
		}
	}
}

bool DMRoom::CheckCollider(const Collider& a, const Collider& b)
{
	if (a.GetMaxX() < b.GetMinX() || a.GetMinX() > b.GetMaxX()) return false;
	if (a.GetMaxY() < b.GetMinY() || a.GetMinY() > b.GetMaxY()) return false;
	return true;
}

void DMRoom::Update()
{
	currentUpdateTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(currentUpdateTime - lastUpdateTime).count();

	GameLogic();
	SendGameState();
	isEnd = EndCheck();
	if (true == isEnd)
	{
		End();
		Logger::Log("��ġ ����, �� �Ŵ����� ���� ��û");
	}
	else
	{
		MiniGameServer::Instance().AddEvent(queueType.second, EV_UPDATE, lastUpdateTime + std::chrono::milliseconds(UPDATE_INTERVAL));
	}
	lastUpdateTime = currentUpdateTime;
}

void DMRoom::SetQueueType(QueueType queType)
{
	queueType = queType;
}

void DMRoom::GameLogic()
{
	UpdateLeftTime();
	UpdatePosition();
	UpdateCollider();
}

void DMRoom::SendGameState()
{
	//Send data to clients.
	for (size_t i = 0; i < characterList.size(); ++i)
	{
		SC_PACKET_CHARACTER_INFO infoPacket{ (UID)i,
			characterList[i]._playerInfo.pos.x, characterList[i]._playerInfo.pos.y, characterList[i]._playerInfo.pos.z,
			characterList[i]._playerInfo.dir.x, characterList[i]._playerInfo.dir.y };
		infoData.EmplaceBack(&infoPacket, infoPacket.size);
	}

	//�̺�Ʈ ������ �޺κп� ����.
	eventData.EmplaceBack(infoData.data, infoData.len);
	for (auto& user : userList)
		MiniGameServer::Instance().SendPacket(user, eventData.data, eventData.len);

	//���� �ʱ�ȭ
	eventData.Clear();
	infoData.Clear();
}

void DMRoom::Regist(std::vector<User*> users)
{
	//���� ��� ó��, UID�� ����.
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (nullptr != users[i])
		{
			users[i]->roomPtr = this;
			userList.emplace_back(users[i]);
			characterList.emplace_back((UID)i, this);

			characterList[i]._playerInfo.initialPos = initialPos[i % _countof(initialPos)];
			characterList[i]._playerInfo.pos = characterList[i]._playerInfo.initialPos;
			characterList[i].SetAbility(users[i]->characterType);

			SC_PACKET_UID packet{ (UID)i };
			MiniGameServer::Instance().SendPacket(users[i], &packet);

			SC_PACKET_SPAWN_CHARACTER spawnCharacterPacket{ (UID)i, users[i]->characterType, users[i]->id, characterList[i]._playerInfo.initialPos.x, characterList[i]._playerInfo.initialPos.y };
			eventData.EmplaceBack(&spawnCharacterPacket, spawnCharacterPacket.size);

			serverID++;
		}
	}

	// [���������� | ���� ���� �ñ׳�] ����
	SC_PACKET_CHANGE_SCENE changeScenePacket{ SCENE_GAME };
	eventData.EmplaceBack(&changeScenePacket, changeScenePacket.size);
	SC_PACKET_TIME timePacket{ (TIME_TYPE)DEFAULT_MATCH_TIME };
	eventData.EmplaceBack(&timePacket, timePacket.size);

	for (auto& user : userList)
		MiniGameServer::Instance().SendPacket(user, eventData.data, eventData.len);
	eventData.Clear();
	return;
}

void DMRoom::Disconnect(User* user)
{
	//������ �뿡�� ���� �� ���� �Ŵ����� �뺸.
	//���нô� ���뺸.
	if (nullptr != user)
	{
		auto iter = std::find(userList.begin(), userList.end(), user);
		if (iter != userList.end())
		{
			//���� �ñ׳��� ���� �� ������? Ȯ���غ���
			UID uid = (UID)std::distance(userList.begin(), iter);
			ProcessReady(uid);

			//���� ����ó��
			user->roomPtr = nullptr;
			userList.erase(iter);
			UserManager::Instance().PushJob(USER_LEAVEROOM, reinterpret_cast<void*>(user->uid));

			//���� ���� �뺸
			SC_PACKET_USER_QUIT quitPacket{ uid };
			eventData.EmplaceBack(&quitPacket, quitPacket.size);
		}
	}
}

void DMRoom::End()
{
	//���� ����ó�� �� �� �Ŵ����� �뺸
	//������ ���ھ�ó���� �̰�����?
	QuitAllUser();
	RoomManager::Instance().PushJob(RMGR_DESTROY, reinterpret_cast<void*>(queueType.second));
}

bool DMRoom::EndCheck()
{
	int leftUserNum = 0;
	for (auto& ch : characterList)
		if (true == ch.IsAlive())
			++leftUserNum;

	if (1 == leftUserNum)
		return true;

	//���ѽð��� �� �Ҹ�Ǹ� ���� ����
	if (0 >= leftTime)
	{
		Logger::Log("�� Ÿ�Ӿƿ�, ��ġ ����");
		return true;
	}

	//������ ��� ������ �����ϸ� ���� ����
	if (true == userList.empty())
	{
		Logger::Log("���� ���� �� 0, ��ġ ����");
		return true;
	}

	return false;
}

void DMRoom::QuitAllUser()
{
	//�����ε� �պκк��� ������. �ϴ� �ִ� �Լ��� Ȱ�������� ���� ���� �ʿ�
	SC_PACKET_CHANGE_SCENE changeScenePacket{ SCENE_MAIN };
	for (auto& user : userList)
		MiniGameServer::Instance().SendPacket(user, &changeScenePacket, changeScenePacket.size);
	for (auto& user : userList)
		Disconnect(user);
}