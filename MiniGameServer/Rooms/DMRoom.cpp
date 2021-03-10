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
	//유저 등록 처리, UID도 전송.
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
		Logger::Log("유저 공격 수신" + std::to_string(reinterpret_cast<UID>(job.second)));
		ProcessAttack(reinterpret_cast<UID>(job.second));
		break;

	case CS_MOVEDIR:
		//Logger::Log("유저 디렉션 수신");
		ProcessMoveDir(reinterpret_cast<MoveDirInfo*>(job.second));
		break;

	default:
		Logger::Log("처리되지 않은 룸 잡 발견");
		break;
	}
}

void DMRoom::Disconnect(User* user)
{
	//유저를 룸에서 삭제 및 유저 매니저에 통보.
	if (nullptr != user)
	{
		user->roomPtr = nullptr;
		UserManager::Instance().PushJob(USER_LEAVEROOM, reinterpret_cast<void*>(user->uid));
	}
}

void DMRoom::End()
{
	//유저 종료처리 및 룸 매니저에 통보
	RoomManager::Instance().PushJob(RMGR_DESTROY, reinterpret_cast<void*>(queueType.second));
}

void DMRoom::Update()
{
	currentUpdateTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(currentUpdateTime - lastUpdateTime).count();

	isEnd = GameLogic();
	SendGameState();
	//Logger::Log("방 업데이트 수행 - " + std::to_string(deltaTime) + "ms");
	if (true == isEnd)
	{
		End();
		Logger::Log("매치 종료, 룸 매니저에 삭제 요청");
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
	// 플레이어 현재 위치 받아오기
	Vector3d pos = character._playerInfo.pos;

	// 플레이어 현재 위치와 넉백 최종위치가 근접하면 콜라이더 넉백 종료
	if (abs(pos.x - character.GetHitCollider()._attackedPos.x) <= 1.f &&
		abs(pos.x - character.GetHitCollider()._attackedPos.x) <= 1.f)
	{
		character.GetHitCollider()._bAttacked = false;
	}

	// 플레이어 현재 위치와 넉백 최종위치 보간 
	character._playerInfo.pos = Vector3d::lerp(pos, character.GetHitCollider()._pos, 10 * deltaTime);
}

void DMRoom::UpdatePos(Character& character)
{
	character._playerInfo.pos += character._playerInfo.dir * character._playerInfo.moveSpeed * deltaTime;
}

void DMRoom::UpdateCollider()
{
	// 공격 당함 체크 
	for (auto& chA : characterList) // 공격하는 플레이어
	{
		if (chA->GetAttackCollider()._enabled == false) continue; // 어택콜라이더 활성화 X -> return
		for (auto& chB : characterList)	// 맞는 플레이어
		{
			if (chA == chB) continue;	// 내 자신은 공격 못한다.
			if (CheckCollider(chA->GetAttackCollider(), chB->GetHitCollider())) // AttackColl, HitColl 충돌 체크
			{
				/* 피격체가 밀려나갈 방향 구하기 */
				Vector3d disVec = chB->_playerInfo.pos - chA->_playerInfo.pos;
				disVec = disVec.normalize();

				/* 피격체의 콜라이더를 피격당한상태로 바꾸고, 밀려날 위치를 부여한다. */
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
	
	//이벤트 데이터 뒷부분에 통합.
	eventData.EmplaceBack(infoData.data, infoData.len);
	for (auto& user : userList)
		MiniGameServer::Instance().SendPacket(user, eventData.data, eventData.len);

	eventData.Clear();
	infoData.Clear();
}