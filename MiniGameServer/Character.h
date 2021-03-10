#pragma once
#include "Utills/Vector3d.h"

struct Collider;

// ��
enum class EState : unsigned char {
	IDLE,
	MOVE,
	ATTACK_READY,
	ATTACK,
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

// �÷��̾� ����
struct FPlayerInfo {
		int id;							// ��Ʈ��ũ id
		Vector3d pos;					//x��ǥ
		Vector3d dir;					//����
		float hp;						//ü��
		float hpm;						//�ִ�ü��
		float attackPower;				//���ݷ�
		float moveSpeed{10.0f};			//�̵��ӵ�
		bool bFlipX;					//true ���� �ٶ󺸴»���
		ESpriteType	sprite;				//sprite ����
		EState		curState;			//���� ����
		float animTime{0.0f};
		EWeaponType 	curWeapon;		// ���� ����
};

// �ݶ��̴�. ĳ���ʹ� attack(��������), hit(�ǰ�����) �� ������ ������ �ִ�.
struct Collider {
	bool	_enabled{false};				// �ݶ��̴� Ȱ��ȭ
	Vector3d _pos{0, 0, 0};					// �ݶ��̴� ��ġ
	float	_width{};						// �ݶ��̴� ����
	float	_height{};

	float _knockBackPower{};				// �˹� ��	(AttackCollider ����)
	float _attackPower{};					// ���ݷ�	(AttackCollider ����)

	bool	_bAttacked{false};				// ���� ���� ���� (HitCollider ����)
	Vector3d _attackedPos{0, 0, 0};			// �з����� ������ ��ġ (HitCollider ����)

	Collider() {}

	Collider(float knockBackPower, float attackPower, float width, float height, Vector3d pos)
	{
		_knockBackPower = knockBackPower;
		_attackPower = attackPower;
		_pos = pos;
		_width = width;
		_height = height;
	}

	float GetMinX() { return _pos.x - _width; }
	float GetMaxX() { return _pos.x + _width; }
	float GetMinY() { return _pos.y - _height; }
	float GetMaxY() { return _pos.y + _height; }
};

/*�÷��̾�*/
class Character
{

public:
	Character();

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
	size_t id{};
	FPlayerInfo _playerInfo;	// �÷��̾� ����
	Collider _hitColl;			// �ǰ� �ݶ��̴�
	Collider _attackColl;		// ���� �ݶ��̴�

	bool operator==(const Character& other) { return id == other.id; };
	void Update(float fTime);
};