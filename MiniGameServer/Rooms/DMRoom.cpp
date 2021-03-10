#include "DMRoom.h"
#include "..\Common\User.h"
#include "..\Utills\Logger.h"
#include "..\RoomManager.h"
#include "..\UserManager.h"
#include "..\MiniGameServer.h"


DMRoom::DMRoom()
{
}

DMRoom::~DMRoom()
{
}

void DMRoom::Init()
{
	lastUpdateTime = std::chrono::high_resolution_clock::now();
	characterList.clear();
	userList.clear();
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
			characterList.emplace_back(new Character);
			characterList.back()->id = i;

			SC_PACKET_SPAWN_CHARACTER spawnCharacterPacket { i, 0, 0, 0 };
			eventData.EmplaceBack(&spawnCharacterPacket, spawnCharacterPacket.size);

			SC_PACKET_UID packet{ i };
			MiniGameServer::Instance().SendPacket(users[i], &packet);
		}
	}
	SC_PACKET_CHANGE_SCENE changeScenePacket{ SCENE_GAME };
	eventData.EmplaceBack(&changeScenePacket, changeScenePacket.size);
	return;
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
		Logger::Log("���� ���� ����" + std::to_string(reinterpret_cast<UID>(job.second)));
		ProcessAttack(reinterpret_cast<UID>(job.second));
		break;

	case CS_MOVEDIR:
		//Logger::Log("���� �𷺼� ����");
		ProcessMoveDir(reinterpret_cast<MoveDirInfo*>(job.second));
		break;

	default:
		Logger::Log("ó������ ���� �� �� �߰�");
		break;
	}
}

void DMRoom::Disconnect(User* user)
{
	//������ �뿡�� ���� �� ���� �Ŵ����� �뺸.
	if (nullptr != user)
	{
		user->roomPtr = nullptr;
		UserManager::Instance().PushJob(USER_LEAVEROOM, reinterpret_cast<void*>(user->uid));
	}
}

void DMRoom::End()
{
	//���� ����ó�� �� �� �Ŵ����� �뺸
	RoomManager::Instance().PushJob(RMGR_DESTROY, reinterpret_cast<void*>(queueType.second));
}

void DMRoom::Update()
{
	currentUpdateTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(currentUpdateTime - lastUpdateTime).count();

	isEnd = GameLogic();
	SendGameState();
	//Logger::Log("�� ������Ʈ ���� - " + std::to_string(deltaTime) + "ms");
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

void DMRoom::ProcessAttack(UID uid)
{
	if (EState::IDLE == characterList[uid]->_playerInfo.curState
		|| EState::MOVE == characterList[uid]->_playerInfo.curState)
	{
		characterList[uid]->_playerInfo.curState = EState::ATTACK_READY;
		SC_PACKET_ATTACK atkPacket{ uid };
		eventData.EmplaceBack(&atkPacket, atkPacket.size);
	}
}

void DMRoom::ProcessMoveDir(MoveDirInfo* info)
{
	if (nullptr == info) return;
	characterList[info->uid]->_playerInfo.dir.x = info->x;
	characterList[info->uid]->_playerInfo.dir.y = info->y;
	delete info;
}

void DMRoom::UpdatePosition()
{
	for (auto& character : characterList)
	{
		character->Update(deltaTime);
		if (true == character->GetHitCollider()._bAttacked)
			KnockBack(*character);
		else
			UpdatePos(*character);
	}
}

void DMRoom::KnockBack(Character& character)
{
	// �÷��̾� ���� ��ġ �޾ƿ���
	Vector3d pos = character._playerInfo.pos;

	// �÷��̾� ���� ��ġ�� �˹� ������ġ�� �����ϸ� �ݶ��̴� �˹� ����
	if (abs(pos.x - character.GetHitCollider()._attackedPos.x) <= 1.f &&
		abs(pos.x - character.GetHitCollider()._attackedPos.x) <= 1.f)
	{
		character.GetHitCollider()._bAttacked = false;
	}

	// �÷��̾� ���� ��ġ�� �˹� ������ġ ���� 
	character._playerInfo.pos = Vector3d::lerp(pos, character.GetHitCollider()._pos, 10 * deltaTime);
}

void DMRoom::UpdatePos(Character& character)
{
	character._playerInfo.pos += character._playerInfo.dir * character._playerInfo.moveSpeed * deltaTime;
}

void DMRoom::UpdateCollider()
{
	// ���� ���� üũ 
	for (auto& chA : characterList) // �����ϴ� �÷��̾�
	{
		if (chA->GetAttackCollider()._enabled == false) continue; // �����ݶ��̴� Ȱ��ȭ X -> return
		for (auto& chB : characterList)	// �´� �÷��̾�
		{
			if (chA == chB) continue;	// �� �ڽ��� ���� ���Ѵ�.
			if (CheckCollider(chA->GetAttackCollider(), chB->GetHitCollider())) // AttackColl, HitColl �浹 üũ
			{
				/* �ǰ�ü�� �з����� ���� ���ϱ� */
				Vector3d disVec = chB->_playerInfo.pos - chA->_playerInfo.pos;
				disVec = disVec.normalize();

				/* �ǰ�ü�� �ݶ��̴��� �ǰݴ��ѻ��·� �ٲٰ�, �з��� ��ġ�� �ο��Ѵ�. */
				chB->GetHitCollider()._bAttacked = true;
				chB->GetHitCollider()._attackedPos = Vector3d(
					chB->_playerInfo.pos.x + (chA->GetAttackCollider()._knockBackPower * disVec.x),
					chB->_playerInfo.pos.y + (chA->GetAttackCollider()._knockBackPower * disVec.x),
					chB->_playerInfo.pos.z
				);
			}
		}
	}
}

bool DMRoom::CheckCollider(Collider& a, Collider& b)
{
	if (a.GetMaxX() < b.GetMinX() || a.GetMinX() > b.GetMaxX()) return false;
	if (a.GetMaxY() < b.GetMinY() || a.GetMinY() > b.GetMaxY()) return false;
	return true;
}

void DMRoom::SetQueueType(QueueType queType)
{
	queueType = queType;
}

bool DMRoom::GameLogic()
{
	UpdatePosition();
	UpdateCollider();
	return false;
}

void DMRoom::SendGameState()
{
	//Send data to clients.
	for (size_t i = 0; i < userList.size(); ++i)
	{
		SC_PACKET_CHARACTER_INFO infoPacket{i, 
			characterList[i]->_playerInfo.pos.x, characterList[i]->_playerInfo.pos.y,
			characterList[i]->_playerInfo.dir.x, characterList[i]->_playerInfo.dir.y};
		infoData.EmplaceBack(&infoPacket, infoPacket.size);
	}
	
	//�̺�Ʈ ������ �޺κп� ����.
	eventData.EmplaceBack(infoData.data, infoData.len);
	for (auto& user : userList)
		MiniGameServer::Instance().SendPacket(user, eventData.data, eventData.len);

	eventData.Clear();
	infoData.Clear();
}