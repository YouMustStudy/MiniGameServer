#pragma once
#include "Utills/Vector3d.h"

struct Collider;

// 상
enum class EState : unsigned char {
	IDLE,
	MOVE,
	ATTACK_READY,
	ATTACK,
	DIE,
	TYPE_END
};

// 무기 종류
enum class EWeaponType : unsigned char {
	Default,
	Gun,
	TYPE_END
};

// 스프라이트 종류
enum class ESpriteType : unsigned char {
	Keke,
	TYPE_END
};

// 플레이어 정보
struct FPlayerInfo {
		int id;							// 네트워크 id
		Vector3d pos;					//x좌표
		Vector3d dir;					//방향
		float hp;						//체력
		float hpm;						//최대체력
		float attackPower;				//공격력
		float moveSpeed{10.0f};			//이동속도
		bool bFlipX;					//true 왼쪽 바라보는상태
		ESpriteType	sprite;				//sprite 종류
		EState		curState;			//현재 상태
		float animTime{0.0f};
		EWeaponType 	curWeapon;		// 현재 무기
};

// 콜라이더. 캐릭터는 attack(공격전용), hit(피격전용) 두 종류를 가지고 있다.
struct Collider {
	bool	_enabled{false};				// 콜라이더 활성화
	Vector3d _pos{0, 0, 0};					// 콜라이더 위치
	float	_width{};						// 콜라이더 범위
	float	_height{};

	float _knockBackPower{};				// 넉백 힘	(AttackCollider 전용)
	float _attackPower{};					// 공격력	(AttackCollider 전용)

	bool	_bAttacked{false};				// 공격 당한 상태 (HitCollider 전용)
	Vector3d _attackedPos{0, 0, 0};			// 밀려나서 가야할 위치 (HitCollider 전용)

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

/*플레이어*/
class Character
{

public:
	Character();

public:
	// 캐릭터 현재 상태 얻기
	EState GetCurState() { return _playerInfo.curState; };

	// 캐릭터 현재 상태 변경
	void SetCurState(const EState& state) { _playerInfo.curState = state; };

	// 어택 콜라이더 활성화
	void SetAttackColliderActive(bool enable) { _attackColl._enabled = enable; };

	// 공격 콜라이더 얻기
	Collider& GetAttackCollider() {
		_attackColl._pos = _playerInfo.pos;
		return _attackColl;
	};
	// 피격 콜라이더 얻기
	Collider& GetHitCollider() {
		_hitColl._pos = _playerInfo.pos;
		return _hitColl;
	};

public:
	size_t id{};
	FPlayerInfo _playerInfo;	// 플레이어 정보
	Collider _hitColl;			// 피격 콜라이더
	Collider _attackColl;		// 공격 콜라이더

	bool operator==(const Character& other) { return id == other.id; };
	void Update(float fTime);
};