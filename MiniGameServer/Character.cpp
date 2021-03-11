#include "Character.h"

Character::Character()
	:_hitColl(0.0f, 0.0f, 100.f, 100.f, Vector3d(0.0, 0.0, 0.0)),
	_attackColl(200.0f, 3.0f, 200.f, 100.f, Vector3d(0.0, 0.0, 0.0))
{
}

void Character::Update(float fTime)
{
	static constexpr float ATK_READY_TIME = 0.1333333f;		//�����غ� �������� 15fps �������� 2������
	static constexpr float ATK_TIME = 0.3333333f;			//���� �������� 15fps �������� 5������

	_playerInfo.animTime += fTime;
	switch (_playerInfo.curState)
	{
	case EState::ATTACK_READY:
		if (ATK_READY_TIME <= _playerInfo.animTime)
		{
			//���ݻ��·� ����
			_playerInfo.curState = EState::ATTACK;
			_playerInfo.animTime -= ATK_READY_TIME;
			_attackColl._enabled = true;
		}
		break;

	case EState::ATTACK:
		if (ATK_TIME <= _playerInfo.animTime)
		{
			//���̵� ���·� ����
			_playerInfo.curState = EState::IDLE;
			_playerInfo.animTime -= ATK_TIME;
			_attackColl._enabled = false;
		}
		break;

	case EState::DIE:
	{
		static const float DROP_SPEED = 1000.0f;
		static const float DEATH_HEIGHT = -500.0f;
		_playerInfo.dropSpeed += DROP_SPEED * fTime;
		_playerInfo.pos.z -= _playerInfo.dropSpeed * fTime;
		if (DEATH_HEIGHT >= _playerInfo.pos.z)
		{
			//���� ���� ���Ϸ� ���������� ������.
			_playerInfo.curState = EState::IDLE;
			_playerInfo.pos = Vector3d{ 0, 0, 0 };
			_playerInfo.dropSpeed = 0.0f;
			_playerInfo.hitCount = 1;
		}
	}
		break;
	}
}
