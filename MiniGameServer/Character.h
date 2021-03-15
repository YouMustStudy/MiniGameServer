#pragma once
#include "Utills/Vector3d.h"
#include "protocol.h"

struct Collider;

// 상
enum class EState : unsigned char {
	IDLE,
	MOVE,
	ATTACK_READY,
	ATTACK,
	FALL,
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

// 오브젝트 종류
enum class EObjectType : unsigned char {
	Player,
	HitEffect,
	TYPE_END
};

// 플레이어 정보
struct FPlayerInfo {
		int id{};						// 네트워크 id
		Vector3d pos{};					//x좌표
		Vector3d initialPos{};
		Vector3d dir{};					//방향

		char life{ 3 };										//목숨
		int hp{3};											//체력
		int hpm{3};											//최대체력
		float hitCount[3]{ 1.0f, 3.0f, 20.0f };				//밀려날 가중치

		float attackPower{200.0f};		//공격력
		float knockbackWeight{ 1.0f };
		float moveSpeed{1000.0f};		//이동속도
		float dropSpeed{0.0f};

		
		ESpriteType	sprite{};			//sprite 종류
		EWeaponType 	curWeapon{};	// 현재 무기
		EState		curState{};			//현재 상태
		float animTime{0.0f};
		
};

// 콜라이더. 캐릭터는 attack(공격전용), hit(피격전용) 두 종류를 가지고 있다.
struct Collider {
	bool	_enabled{false};				// 콜라이더 활성화
	Vector3d _pos{0, 0, 0};					// 콜라이더 위치
	float	_width{};						// 콜라이더 범위
	float	_height{};

	bool	_bAttacked{false};				// 공격 당한 상태 (HitCollider 전용)
	Vector3d _attackedPos{0, 0, 0};			// 밀려나서 가야할 위치 + 대쉬 위치 (HitCollider 전용)

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

/*플레이어*/
class DMRoom;
class Character
{
public:
	Character(size_t id, DMRoom* roomPtr);

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
	FPlayerInfo _playerInfo;	// 플레이어 정보
	Collider _hitColl;			// 피격 콜라이더
	Collider _attackColl;		// 공격 콜라이더
	size_t id{(size_t)-1};
	DMRoom* roomPtr{nullptr};

	bool operator==(const Character& other) { return id == other.id; };
	void Update(float fTime);
	void GetDamage(UID attacker, int damage);
	void SetAbility(unsigned char characterType);
	bool IsAlive() { return 0 < _playerInfo.life; };

private:
	void KnockBack(float fTime);
	void UpdatePos(float fTime);
	void UpdateState(float fTime);
	void ChangeHP(int hp);
};