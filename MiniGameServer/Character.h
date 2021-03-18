#pragma once
#include "Utills/Vector3d.h"
#include "protocol.h"
#include "ServerConfig.h"

struct Collider;

// ��
enum class EState : unsigned char {
	IDLE,
	MOVE,
	ATTACK_READY,
	ATTACK,
	FALL,
	DIE,
	TYPE_END
};

// ���� ����
enum class EWeaponType : unsigned char {
	Default,
	Gun,
	TYPE_END
};

// ��������Ʈ ����
enum class ESpriteType : unsigned char {
	Keke,
	TYPE_END
};

// ������Ʈ ����
enum class EObjectType : unsigned char {
	Player,
	HitEffect,
	TYPE_END
};

// �÷��̾� ����
struct FPlayerInfo {
		bool isReady{ false };			// �ε� �ϷῩ��

		Vector3d pos{};					//x��ǥ
		Vector3d initialPos{};
		Vector3d dir{};					//����

		char life{ CHARACTER_LIFE };						//���
		int hitPoint{ 0 };
		float invincibleTime{ INVINCIBLE_TIME };			//���� Ÿ�̸�

		float attackPower{ CHARACTER_ATTACK_POWER };			//���ݷ�
		float knockbackWeight{ CHARACTER_KNOCKBACK_WEIGHT };
		float moveSpeed{ CHARACTER_MOVE_SPEED };				//�̵��ӵ�
		float dropSpeed{ CHARACTER_DROP_SPEED };

		ESpriteType	sprite{};			//sprite ����
		EWeaponType 	curWeapon{};	// ���� ����
		EState		curState{};			//���� ����
		float animTime{0.0f};

		bool isBomb = {false};
		float curBombTime{ BOMB_TIME };
};

// �ݶ��̴�. ĳ���ʹ� attack(��������), hit(�ǰ�����) �� ������ ������ �ִ�.
struct Collider {
	bool	_enabled{false};				// �ݶ��̴� Ȱ��ȭ
	Vector3d _pos{0, 0, 0};					// �ݶ��̴� ��ġ
	float	_width{};						// �ݶ��̴� ����
	float	_height{};

	bool	_bAttacked{false};				// ���� ���� ���� (HitCollider ����)
	Vector3d _attackedPos{0, 0, 0};			// �з����� ������ ��ġ + �뽬 ��ġ (HitCollider ����)

	Collider(float width, float height, Vector3d pos)
	{
		_pos = pos;
		_width = width;
		_height = height;
	}

	float GetMinX() const { return _pos.x - _width; }
	float GetMaxX() const { return _pos.x + _width; }
	float GetMinY() const { return _pos.y - _height; }
	float GetMaxY() const { return _pos.y + _height; }
};

/*�÷��̾�*/
class DMRoom;
class Character
{
	friend DMRoom;

public:
	Character(UID id, DMRoom* roomPtr);

public:
	// ĳ���� ���� ���� ���
	EState GetCurState() { return _playerInfo.curState; };

	// ĳ���� ���� ���� ����
	void SetCurState(const EState& state) { _playerInfo.curState = state; };

	// ���� �ݶ��̴� Ȱ��ȭ
	void SetAttackColliderActive(bool enable) { _attackColl._enabled = enable; };

	// ���� �ݶ��̴� ���
	Collider& GetAttackCollider() {
		_attackColl._pos = _playerInfo.pos;
		return _attackColl;
	};

	// �ǰ� �ݶ��̴� ���
	Collider& GetHitCollider() {
		_hitColl._pos = _playerInfo.pos;
		return _hitColl;
	};

public:
	FPlayerInfo _playerInfo;	// �÷��̾� ����
	Collider _hitColl;			// �ǰ� �ݶ��̴�
	Collider _attackColl;		// ���� �ݶ��̴�
	UID id{(UID)-1};			// UID
	DMRoom* roomPtr{nullptr};	// �濡 �۾� �߰��� ������

	bool operator==(const Character& other) { return id == other.id; };
	void Update(float fTime);	//ĳ���� ������Ʈ
	void GetDamage(UID attacker);	//������ ó��
	void SetAbility(CHARACTER_TYPE characterType);	//ĳ���� Ÿ�Կ� ���� �ʱ⽺�� ����
	bool IsAlive() { 
		if(_playerInfo.isBomb == true) return false;
		return 0 < _playerInfo.life; 
	};	//�� ĳ���Ͱ� ����ִ°�?
	bool IsInvincible() { return INVINCIBLE_TIME > _playerInfo.invincibleTime; };	//�� ĳ���Ͱ� �����ΰ�?
	
private:
	void KnockBack(float fTime);
	void UpdatePos(float fTime);
	void UpdateState(float fTime);
	void ChangeHitPoint(int point);
};