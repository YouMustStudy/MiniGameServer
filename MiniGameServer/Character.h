#pragma once
#include "Utills/Vector3d.h"
#include "protocol.h"
#include "ServerConfig.h"
#include "Common/User.h"

struct Collider;

// ��
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

// ���� ����
enum class EWeaponType : unsigned char
{
	Default,
	Gun,
	TYPE_END
};

// ��������Ʈ ����
enum class ESpriteType : unsigned char
{
	Keke,
	TYPE_END
};

// ������Ʈ ����
enum class EObjectType : unsigned char
{
	Player,
	HitEffect,
	TYPE_END
};

/**
*@brief �÷��̾� ����
*@author Byeoungjun Moon.
*/
struct FPlayerInfo 
{
	bool     isReady{ false };                              ///< �ε� �ϷῩ��
	Vector3d pos{};			                                ///< ��ġ
	Vector3d initialPos{};                                  ///< ���� ���� ��ġ
	Vector3d dir{};			                                ///< ����
	char     life{ CHARACTER_LIFE };                        ///< ���
	int      hitPoint{ 0 };                                 ///< �ǰ� Ƚ��
	float    invincibleTime{ INVINCIBLE_TIME };             ///< ���� Ÿ�̸�

	float    attackPower{ CHARACTER_ATTACK_POWER };			///< ���ݷ�
	float    moveSpeed  { CHARACTER_MOVE_SPEED   };			///< �̵��ӵ�
	float    dropSpeed  { CHARACTER_DROP_SPEED   };         ///< ���ϼӵ�
	float    knockbackWeight{ CHARACTER_KNOCKBACK_WEIGHT }; ///< �̴� ��

	ESpriteType	sprite   {};	                            ///< sprite ����
	EWeaponType	curWeapon{};	                            ///< ���� ����
	EState		curState {};	                            ///< ���� ����
	float       animTime { 0.0f };                          ///< �ִϸ��̼� �ð�

	bool  isBomb{ false };                                  ///< ��ź ����
	float curBombTime{ BOMB_TIME };                         ///< ��ź �ð�
};


/**
*@brief �ݶ��̴�. ĳ���ʹ� attack(��������), hit(�ǰ�����) �� ������ ������ �ִ�.
*@author Byeoungjun Moon.
*/
struct Collider {
	bool	 _enabled{ false   };     ///< �ݶ��̴� Ȱ��ȭ
	Vector3d _pos    { 0, 0, 0 };     ///< �ݶ��̴� ��ġ
	float	 _width{};			      ///< �ݶ��̴� ����
	float	 _height{};               ///< �ݶ��̴� ����

	bool	 _bAttacked  { false   }; ///< ���� ���� ����                   (HitCollider ����)
	Vector3d _attackedPos{ 0, 0, 0 }; ///< �з����� ������ ��ġ + �뽬 ��ġ (HitCollider ����)

	/**
	*@brief �⺻ ������.
	*/
	Collider( float width, float height, Vector3d pos, bool enabled )
	{
		_pos     = pos;
		_width   = width;
		_height  = height;
		_enabled = enabled;
	}

	/**
	*@brief ���� ���� x��ǥ�� ��ȯ�Ѵ�.
	*@return ��ǥ
	*/
	float GetMinX() const { return _pos.x - _width;  }

	/**
	*@brief ���� ū x��ǥ�� ��ȯ�Ѵ�.
	*@return ��ǥ
	*/
	float GetMaxX() const { return _pos.x + _width;  }

	/**
	*@brief ���� ���� y��ǥ�� ��ȯ�Ѵ�.
	*@return ��ǥ
	*/
	float GetMinY() const { return _pos.y - _height; }

	/**
	*@brief ���� ū y��ǥ�� ��ȯ�Ѵ�.
	*@return ��ǥ
	*/
	float GetMaxY() const { return _pos.y + _height; }
};


/**
*@brief ĳ���� ����.
*@author Byeoungjun Moon.
*/
class DMRoom;
class Character
{
	friend DMRoom;

public:
	/**
	*@brief �⺻ ������.
	*/
	Character( UID id, DMRoom* roomPtr );

public:
	/**
	*@brief ĳ������ ���� ���¸� ��ȯ�Ѵ�.
	*@return ĳ������ ���� ����.
	*/
	EState GetCurState() { return _playerInfo.curState; };

	/**
	*@brief ĳ������ ���� ���¸� �����Ѵ�.
	*@param[in] state ĳ������ ����.
	*/
	void SetCurState( EState state ) { _playerInfo.curState = state; };

	/**
	*@brief ���� �ݶ��̴��� ���¸� �����Ѵ�.
	*@param[in] enable Ȱ��ȭ ����.
	*/
	void SetAttackColliderActive( bool enable ) { _attackColl._enabled = enable; };

	/**
	*@brief ���� �ݶ��̴��� ��ȯ�Ѵ�.
	*@return ���� �ݶ��̴�.
	*/
	Collider& GetAttackCollider() {
		_attackColl._pos = _playerInfo.pos;
		return _attackColl;
	};

	/**
	*@brief �ǰ� �ݶ��̴��� ��ȯ�Ѵ�.
	*@return �ǰ� �ݶ��̴�.
	*/
	Collider& GetHitCollider() {
		_hitColl._pos = _playerInfo.pos;
		return _hitColl;
	};

public:
	FPlayerInfo _playerInfo;	    ///< �÷��̾� ����
	Collider    _hitColl;		    ///< �ǰ� �ݶ��̴�
	Collider    _attackColl;	    ///< ���� �ݶ��̴�
	UID         id{ (UID)-1 };		///< UID
	DMRoom*     roomPtr{ nullptr };	///< ��
	User*       userPtr{ nullptr }; ///< ����

	/**
	*@brief == ������
	*/
	bool operator==( const Character& other ) { return id == other.id; };

	/**
	*@brief ĳ���͸� ������Ʈ�Ѵ�.
	*@param[in] fTime ��ȭ�� �ð�.
	*/
	void Update( float fTime );
	
	/**
	*@brief ������ ó���� �Ѵ�.
	*@param[in] attacker �������� UID.
	*/
	void GetDamage( UID attacker );

	/**
	*@brief ĳ������ �ʱ� ������ �����Ѵ�.
	*@param[in] characterType ĳ���� Ÿ��.
	*/
	void SetAbility( CHARACTER_TYPE characterType );

	/**
	*@brief ���� ���θ� ��ȯ�Ѵ�.
	*@return ���� ����.
	*/
	bool IsAlive();

	/**
	*@brief ���� ���θ� ��ȯ�Ѵ�.
	*@return ���� ����.
	*/
	bool IsInvincible() { return INVINCIBLE_TIME > _playerInfo.invincibleTime; };

private:

	/**
	*@brief �˹� ó���� �Ѵ�.
	*@param[in] fTime ��ȭ�� �ð�.
	*/
	void KnockBack( float fTime );

	/**
	*@brief ��ġ ó���� �Ѵ�.
	*@param[in] fTime ��ȭ�� �ð�.
	*/
	void UpdatePos( float fTime );

	/**
	*@brief ���º�ȭ ó���� �Ѵ�.
	*@param[in] fTime ��ȭ�� �ð�.
	*/
	void UpdateState( float fTime );

	/**
	*@brief �ǰ� Ƚ���� �����Ѵ�.
	*@param[in] point �ǰ� Ƚ��.
	*/
	void ChangeHitPoint( int point );
};