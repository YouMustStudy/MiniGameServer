#include "Character.h"

Character::Character()
	:_hitColl(0.0f, 0.0f, 100.f, 100.f, Vector3d(0.0, 0.0, 0.0)),
	_attackColl(200.0f, 3.0f, 200.f, 100.f, Vector3d(0.0, 0.0, 0.0))
{
}

void Character::Update(float fTime)
{
	static constexpr float ATK_READY_TIME = 0.1333333f;		//공격준비 프레임은 15fps 기준으로 2프레임
	static constexpr float ATK_TIME = 0.3333333f;			//공격 프레임은 15fps 기준으로 5프레임

	_playerInfo.animTime += fTime;
	switch (_playerInfo.curState)
	{
	case EState::ATTACK_READY:
		if (ATK_READY_TIME <= _playerInfo.animTime)
		{
			//공격상태로 전이
			_playerInfo.curState = EState::ATTACK;
			_playerInfo.animTime -= ATK_READY_TIME;
			_attackColl._enabled = true;
		}
		break;

	case EState::ATTACK:
		if (ATK_TIME <= _playerInfo.animTime)
		{
			//아이들 상태로 전이
			_playerInfo.curState = EState::IDLE;
			_playerInfo.animTime -= ATK_TIME;
			_attackColl._enabled = false;
		}
		break;
	}
}
