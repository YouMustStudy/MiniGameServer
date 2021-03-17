#include "Character.h"
#include "Rooms/DMRoom.h"

Character::Character(UID id, DMRoom* roomPtr)
	:_hitColl(CHARACTER_HITBOX_WIDTH, CHARACTER_HITBOX_HEIGHT, Vector3d(0.0, 0.0, 0.0)),
	_attackColl(ATTACK_HITBOX_WIDTH, ATTACK_HITBOX_HEIGHT, Vector3d(0.0, 0.0, 0.0)),
	id(id), roomPtr(roomPtr)
{
}

void Character::Update(float fTime)
{
	UpdateState(fTime);
	if (true == GetHitCollider()._bAttacked)
		KnockBack(fTime);
	else if (EState::DIE != _playerInfo.curState
		&& EState::FALL != _playerInfo.curState)
		UpdatePos(fTime);
}

void Character::KnockBack(float fTime)
{
	// 플레이어 현재 위치 받아오기
	Vector3d pos = _playerInfo.pos;

	// 플레이어 현재 위치와 넉백 최종위치가 근접하면 콜라이더 넉백 종료
	if (abs(pos.x - GetHitCollider()._attackedPos.x) <= 1.f &&
		abs(pos.y - GetHitCollider()._attackedPos.y) <= 1.f)
	{
		GetHitCollider()._bAttacked = false;
	}

	// 플레이어 현재 위치와 넉백 최종위치 보간, Z는 낙하를 위해 제외.
	Vector3d dst = Vector3d::lerp(pos, GetHitCollider()._attackedPos, 10.0f * fTime);
	_playerInfo.pos.x = dst.x;
	_playerInfo.pos.y = dst.y;
}

void Character::UpdatePos(float fTime)
{
	_playerInfo.pos += _playerInfo.dir * _playerInfo.moveSpeed * fTime;
}

void Character::SetAbility(unsigned char characterType)
{
	switch (characterType)
	{
	case 0: //인간
		_playerInfo.knockbackWeight *= 1.0f;
		_playerInfo.attackPower *= 1.0f;
		_playerInfo.moveSpeed *= 1.0f;
		break;

	case 1: //엘프
		_playerInfo.knockbackWeight *= 1.25f;
		_playerInfo.attackPower *= 0.7f;
		_playerInfo.moveSpeed *= 1.2f;
		break;

	case 2: //드워프
		_playerInfo.knockbackWeight *= 1.67f;
		_playerInfo.attackPower *= 1.4f;
		_playerInfo.moveSpeed *= 0.8f;
		break;

	case 3: //다크엘프
		_playerInfo.knockbackWeight *= 1.0f;
		_playerInfo.attackPower *= 0.8f;
		_playerInfo.moveSpeed *= 1.1f;
		break;
	}
}

void Character::UpdateState(float fTime)
{
	_playerInfo.animTime += fTime;
	_playerInfo.invincibleTime += fTime;
	switch (_playerInfo.curState)
	{
	case EState::ATTACK_READY:
	{
		if (ATK_READY_TIME <= _playerInfo.animTime)
		{
			//공격상태로 전이
			_playerInfo.curState = EState::ATTACK;
			_playerInfo.animTime -= ATK_READY_TIME;
			_attackColl._enabled = true;
		}
		break;
	}

	case EState::ATTACK:
	{
		if (ATK_TIME <= _playerInfo.animTime)
		{
			//아이들 상태로 전이
			_playerInfo.curState = EState::IDLE;
			_playerInfo.animTime -= ATK_TIME;
			_attackColl._enabled = false;
		}
		break;
	}

	case EState::FALL: //맵밖으로 떨어지는중
	{
		_playerInfo.dropSpeed += DROP_SPEED * fTime;
		_playerInfo.pos.z -= _playerInfo.dropSpeed * fTime;
		if (DEATH_HEIGHT >= _playerInfo.pos.z)
		{
			_playerInfo.curState = EState::DIE;
			_hitColl._bAttacked = false;
			_playerInfo.animTime = 0.0f;
			_playerInfo.dropSpeed = CHARACTER_DROP_SPEED;
			_playerInfo.pos.x = WAIT_RESPAWN_SPACE;
			_playerInfo.pos.y = WAIT_RESPAWN_SPACE;
			_playerInfo.pos.z = DEATH_HEIGHT;
			SC_PACKET_CHARACTER_INFO teleportPacket{ id,
			WAIT_RESPAWN_SPACE, WAIT_RESPAWN_SPACE, DEATH_HEIGHT,
			_playerInfo.dir.x, _playerInfo.dir.y, true };

			if (nullptr != roomPtr)
				roomPtr->infoData.EmplaceBack(&teleportPacket, teleportPacket.size);

			//남은 목숨 수 중계
			if (0 < _playerInfo.life)
			{
				--_playerInfo.life;
				SC_PACKET_CHANGE_LIFE lifePacket{ id, _playerInfo.life };
				if (nullptr != roomPtr)
					roomPtr->infoData.EmplaceBack(&lifePacket, lifePacket.size);
			}
		}
		break;
	}

	case EState::DIE: //리스폰 대기
	{
		if (0 < _playerInfo.life)
		{
			if (RESPAWN_TIME <= _playerInfo.animTime)
			{
				_hitColl._bAttacked = false;
				_playerInfo.curState = EState::IDLE;
				_playerInfo.pos = _playerInfo.initialPos;
				_playerInfo.animTime = 0.0f;
				_playerInfo.invincibleTime = 0.0f;
				ChangeHitPoint(0);

				SC_PACKET_CHARACTER_INFO teleportPacket{ id,
					_playerInfo.pos.x, _playerInfo.pos.y, _playerInfo.pos.z,
					_playerInfo.dir.x, _playerInfo.dir.y, true };
				if (nullptr != roomPtr)
					roomPtr->infoData.EmplaceBack(&teleportPacket, teleportPacket.size);
			}
		}
		break;
	}

	}
}

void Character::GetDamage(UID attacker)
{
	++_playerInfo.hitPoint;
	SC_PACKET_CHANGE_HP changeHPPacket{ id ,_playerInfo.hitPoint, attacker };
	if (nullptr != roomPtr)
		roomPtr->eventData.EmplaceBack(&changeHPPacket, changeHPPacket.size);
}

void Character::ChangeHitPoint(int point)
{
	if (0 <= point)
	{
		_playerInfo.hitPoint = point;
		SC_PACKET_CHANGE_HP changeHPPacket{ id ,_playerInfo.hitPoint };
		if (nullptr != roomPtr)
			roomPtr->eventData.EmplaceBack(&changeHPPacket, changeHPPacket.size);
	}
}
