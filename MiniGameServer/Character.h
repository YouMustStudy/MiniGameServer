#pragma once
#include "Utills/Vector3d.h"
#include "protocol.h"
#include "ServerConfig.h"
#include "Common/User.h"

struct Collider;

// 상
enum class EState : unsigned char
{
	IDLE,
	MOVE,
	ATTACK_READY,
	ATTACK,
	FALL,
	DIE,
	TYPE_END
};

// 무기 종류
enum class EWeaponType : unsigned char
{
	Default,
	Gun,
	TYPE_END
};

// 스프라이트 종류
enum class ESpriteType : unsigned char
{
	Keke,
	TYPE_END
};

// 오브젝트 종류
enum class EObjectType : unsigned char
{
	Player,
	HitEffect,
	TYPE_END
};

/**
*@brief 플레이어 정보
*@author Byeoungjun Moon.
*/
struct FPlayerInfo 
{
	bool     isReady{ false };                              ///< 로딩 완료여부
	Vector3d pos{};			                                ///< 위치
	Vector3d initialPos{};                                  ///< 최초 스폰 위치
	Vector3d dir{};			                                ///< 방향
	char     life{ CHARACTER_LIFE };                        ///< 목숨
	int      hitPoint{ 0 };                                 ///< 피격 횟수
	float    invincibleTime{ INVINCIBLE_TIME };             ///< 무적 타이머

	float    attackPower{ CHARACTER_ATTACK_POWER };			///< 공격력
	float    moveSpeed  { CHARACTER_MOVE_SPEED   };			///< 이동속도
	float    dropSpeed  { CHARACTER_DROP_SPEED   };         ///< 낙하속도
	float    knockbackWeight{ CHARACTER_KNOCKBACK_WEIGHT }; ///< 미는 힘

	ESpriteType	sprite   {};	                            ///< sprite 종류
	EWeaponType	curWeapon{};	                            ///< 무기 종류
	EState		curState {};	                            ///< 현재 상태
	float       animTime { 0.0f };                          ///< 애니메이션 시간

	bool  isBomb{ false };                                  ///< 폭탄 여부
	float curBombTime{ BOMB_TIME };                         ///< 폭탄 시간
};


/**
*@brief 콜라이더. 캐릭터는 attack(공격전용), hit(피격전용) 두 종류를 가지고 있다.
*@author Byeoungjun Moon.
*/
struct Collider {
	bool	 _enabled{ false   };     ///< 콜라이더 활성화
	Vector3d _pos    { 0, 0, 0 };     ///< 콜라이더 위치
	float	 _width{};			      ///< 콜라이더 넓이
	float	 _height{};               ///< 콜라이더 높이

	bool	 _bAttacked  { false   }; ///< 공격 당한 상태                   (HitCollider 전용)
	Vector3d _attackedPos{ 0, 0, 0 }; ///< 밀려나서 가야할 위치 + 대쉬 위치 (HitCollider 전용)

	/**
	*@brief 기본 생성자.
	*/
	Collider( float width, float height, Vector3d pos, bool enabled )
	{
		_pos     = pos;
		_width   = width;
		_height  = height;
		_enabled = enabled;
	}

	/**
	*@brief 가장 작은 x좌표를 반환한다.
	*@return 좌표
	*/
	float GetMinX() const { return _pos.x - _width;  }

	/**
	*@brief 가장 큰 x좌표를 반환한다.
	*@return 좌표
	*/
	float GetMaxX() const { return _pos.x + _width;  }

	/**
	*@brief 가장 작은 y좌표를 반환한다.
	*@return 좌표
	*/
	float GetMinY() const { return _pos.y - _height; }

	/**
	*@brief 가장 큰 y좌표를 반환한다.
	*@return 좌표
	*/
	float GetMaxY() const { return _pos.y + _height; }
};


/**
*@brief 캐릭터 정보.
*@author Byeoungjun Moon.
*/
class DMRoom;
class Character
{
	friend DMRoom;

public:
	/**
	*@brief 기본 생성자.
	*/
	Character( UID id, DMRoom* roomPtr );

public:
	/**
	*@brief 캐릭터의 현재 상태를 반환한다.
	*@return 캐릭터의 현재 상태.
	*/
	EState GetCurState() { return _playerInfo.curState; };

	/**
	*@brief 캐릭터의 현재 상태를 변경한다.
	*@param[in] state 캐릭터의 상태.
	*/
	void SetCurState( EState state ) { _playerInfo.curState = state; };

	/**
	*@brief 공격 콜라이더의 상태를 설정한다.
	*@param[in] enable 활성화 상태.
	*/
	void SetAttackColliderActive( bool enable ) { _attackColl._enabled = enable; };

	/**
	*@brief 공격 콜라이더를 반환한다.
	*@return 공격 콜라이더.
	*/
	Collider& GetAttackCollider() {
		_attackColl._pos = _playerInfo.pos;
		return _attackColl;
	};

	/**
	*@brief 피격 콜라이더를 반환한다.
	*@return 피격 콜라이더.
	*/
	Collider& GetHitCollider() {
		_hitColl._pos = _playerInfo.pos;
		return _hitColl;
	};

public:
	FPlayerInfo _playerInfo;	    ///< 플레이어 정보
	Collider    _hitColl;		    ///< 피격 콜라이더
	Collider    _attackColl;	    ///< 공격 콜라이더
	UID         id{ (UID)-1 };		///< UID
	DMRoom*     roomPtr{ nullptr };	///< 룸
	User*       userPtr{ nullptr }; ///< 유저

	/**
	*@brief == 재정의
	*/
	bool operator==( const Character& other ) { return id == other.id; };

	/**
	*@brief 캐릭터를 업데이트한다.
	*@param[in] fTime 변화한 시간.
	*/
	void Update( float fTime );
	
	/**
	*@brief 데미지 처리를 한다.
	*@param[in] attacker 공격자의 UID.
	*/
	void GetDamage( UID attacker );

	/**
	*@brief 캐릭터의 초기 정보를 설정한다.
	*@param[in] characterType 캐릭터 타입.
	*/
	void SetAbility( CHARACTER_TYPE characterType );

	/**
	*@brief 생존 여부를 반환한다.
	*@return 생존 여부.
	*/
	bool IsAlive();

	/**
	*@brief 무적 여부를 반환한다.
	*@return 무적 여부.
	*/
	bool IsInvincible() { return INVINCIBLE_TIME > _playerInfo.invincibleTime; };

private:

	/**
	*@brief 넉백 처리를 한다.
	*@param[in] fTime 변화한 시간.
	*/
	void KnockBack( float fTime );

	/**
	*@brief 위치 처리를 한다.
	*@param[in] fTime 변화한 시간.
	*/
	void UpdatePos( float fTime );

	/**
	*@brief 상태변화 처리를 한다.
	*@param[in] fTime 변화한 시간.
	*/
	void UpdateState( float fTime );

	/**
	*@brief 피격 횟수를 설정한다.
	*@param[in] point 피격 횟수.
	*/
	void ChangeHitPoint( int point );
};