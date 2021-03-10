#include "Character.h"

Character::Character()
	:_hitColl(0.0f, 0.0f, 100.f, 100.f, Vector3d(0.0, 0.0, 0.0)),
	_attackColl(200.0f, 3.0f, 200.f, 100.f, Vector3d(0.0, 0.0, 0.0))
{
}

void Character::Update(float fTime)
{
	static constexpr float ATK_READY_TIME = 0.1333333f;
	static constexpr float ATK_TIME = 0.3333333f;

	_playerInfo.animTime += fTime;
	switch (_playerInfo.curState)
	{
	case EState::ATTACK_READY:
		if (ATK_READY_TIME <= _playerInfo.animTime)
		{
			_playerInfo.curState = EState::ATTACK;
			_playerInfo.animTime -= ATK_READY_TIME;
			_attackColl._enabled = true;
		}
		break;

	case EState::ATTACK:
		if (ATK_TIME <= _playerInfo.animTime)
		{
			_playerInfo.curState = EState::IDLE;
			_playerInfo.animTime -= ATK_TIME;
			_attackColl._enabled = false;
		}
		break;
	}
}
