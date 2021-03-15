#include "Character.h"
#include "Rooms/DMRoom.h"

Character::Character(size_t id, DMRoom* roomPtr)
	:_hitColl(100.f, 100.f, Vector3d(0.0, 0.0, 0.0)),
	_attackColl(200.f, 100.f, Vector3d(0.0, 0.0, 0.0)),
	id(id), roomPtr(roomPtr)
{
}

void Character::Update(float fTime)
{
	UpdateState(fTime);
	if (true == GetHitCollider()._bAttacked)
		KnockBack(fTime);
	else
		UpdatePos(fTime);
}

void Character::KnockBack(float fTime)
{
	// �÷��̾� ���� ��ġ �޾ƿ���
	Vector3d pos = _playerInfo.pos;

	// �÷��̾� ���� ��ġ�� �˹� ������ġ�� �����ϸ� �ݶ��̴� �˹� ����
	if (abs(pos.x - GetHitCollider()._attackedPos.x) <= 1.f &&
		abs(pos.y - GetHitCollider()._attackedPos.y) <= 1.f)
	{
		GetHitCollider()._bAttacked = false;
	}

	// �÷��̾� ���� ��ġ�� �˹� ������ġ ����, Z�� ���ϸ� ���� ����.
	Vector3d dst = Vector3d::lerp(pos, GetHitCollider()._attackedPos, 10.0f * fTime);
	_playerInfo.pos.x = dst.x;
	_playerInfo.pos.y = dst.y;
}

void Character::UpdatePos(float fTime)
{
	_playerInfo.pos += _playerInfo.dir * _playerInfo.moveSpeed * fTime;
}

void Character::UpdateState(float fTime)
{
	//���� 1 / fps * animation frame
	static constexpr float ATK_READY_TIME = 0.1333333f;		//�����غ� �������� 15fps �������� 2������
	static constexpr float ATK_TIME = 0.3333333f;			//���� �������� 15fps �������� 5������
	static const float DROP_SPEED = 10000.0f; // �߷�
	static const float DEATH_HEIGHT = -500.0f; // �״� ����
	static const float RESPAWN_TIME = 3.0f; //������ �ð�

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

	case EState::FALL: //�ʹ����� ����������
	{
		_playerInfo.dropSpeed += DROP_SPEED * fTime;
		_playerInfo.pos.z -= _playerInfo.dropSpeed * fTime;
		if (DEATH_HEIGHT >= _playerInfo.pos.z)
		{
			_playerInfo.animTime = 0.0f;
			_playerInfo.curState = EState::DIE;
			_playerInfo.dropSpeed = 0.0f;
			_playerInfo.pos.x = 0.0f;
			_playerInfo.pos.y = 0.0f;
		}
		break;
	}

	case EState::DIE: //������ ���
	{
		if (RESPAWN_TIME <= _playerInfo.animTime)
		{
			_playerInfo.curState = EState::IDLE;
			_playerInfo.pos = Vector3d{ 0, 0, 0 };
			_hitColl._bAttacked = false;
			_playerInfo.animTime = 0.0f;
			ChangeHP(_playerInfo.hpm);
		}
		break;
	}
	}
}

void Character::ChangeHP(int hp)
{
	if (0 <= hp)
	{
		_playerInfo.hp = hp;
		SC_PACKET_CHANGE_HP changeHPPacket{ id ,_playerInfo.hp };
		if (nullptr != roomPtr)
			roomPtr->eventData.EmplaceBack(&changeHPPacket, changeHPPacket.size);
	}
}

void Character::GetDamage(UID attacker, int damage)
{
	damage = (damage < _playerInfo.hp) ? damage : _playerInfo.hp;
	int afterHP = _playerInfo.hp - damage;
	if (0 <= afterHP)
	{
		_playerInfo.hp = afterHP;
		SC_PACKET_CHANGE_HP changeHPPacket{ id ,_playerInfo.hp, attacker };
		if (nullptr != roomPtr)
			roomPtr->eventData.EmplaceBack(&changeHPPacket, changeHPPacket.size);
	}
}