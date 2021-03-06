#pragma once
#include <string>
#include <cassert>

//Server -> Client.
enum SC_PACKET
{
	//Main
	SC_CONNECT_OK,				///< 연결 성공
	SC_DISCONNECT,				///< 서버에서 연결 종료
	SC_LOGIN_OK,				///< 로그인 성공
	SC_LOGIN_FAIL,				///< 로그인 실패
	SC_CHANGE_QUEUE,			///< 매치큐 대기상태 변경
	SC_CHANGE_SCENE,			///< 게임시작, 종료를 알림

	//Game
	SC_UID,						///< 유저에게 게임 UID 전송
	SC_TIME,					///< 서버/클라 시간 동기화
	SC_SPAWN_CHARACTER,			///< 캐릭터 오브젝트 추가
	SC_DESTROY_CHARACTER,		///< 캐릭터 오브젝트 삭제
	SC_ATTACK,					///< 캐릭터 공격
	SC_CHANGE_HP,				///< 캐릭터 체력 변경
	SC_CHANGE_LIFE,				///< 캐릭터 목숨 변경
	SC_CHARACTER_INFO,			///< 캐릭터의 위치, 방향 등 값
	SC_SPAWN_EFFECT,			///< 타격모션 등 이펙트 출력
	SC_USER_QUIT,				///< 게임 중 다른 유저의 종료를 통보
	SC_SPAWN_BOMB,				///< 폭탄 스폰
	SC_COUNT
};

//Client -> Server.
enum CS_PACKET
{
	//To UserManager
	CS_REQUEST_LOGIN,			///< 로그인 요청
	CS_ENQUEUE,					///< 매치큐 등록 요청
	CS_DEQUEUE,					///< 매치큐 등록해제 요청

	//To Room
	CS_UPDATE,					///< 방 업데이트 요청
	CS_LEAVEROOM,				///< 유저 접속 종료 요청
	CS_ATTACK,					///< 유저 공격요청
	CS_MOVEDIR,					///< 유저 움직임 요청
	CS_READY,					///< 3,2,1 카운트 다 셌음
	CS_COUNT
};

enum SCENE_TYPE
{
	SCENE_MAIN,
	SCENE_GAME
};

//타입 정의.
using PACKET_TYPE       = uint8_t;
using PACKET_SIZE       = uint16_t;
using UID               = uint64_t;
using SCENETYPE         = uint8_t;
using CHARACTER_TYPE    = uint8_t;
using TIME_TYPE         = uint16_t;
using LOGIN_FAIL_REASON = uint8_t;
constexpr size_t NAME_LENGTH = 20;

#pragma pack(1)
class DEFAULT_PACKET
{
public:
	PACKET_SIZE size{ sizeof( DEFAULT_PACKET ) }; ///< 패킷 크기.
	PACKET_TYPE type{ (PACKET_TYPE)-1 };          ///< 패킷 종류.
};

class SC_PACKET_CONNECT_OK : public DEFAULT_PACKET
{
public:
	SC_PACKET_CONNECT_OK()
	{
		size = sizeof( SC_PACKET_CONNECT_OK );
		type = SC_CONNECT_OK;
	};
};

class SC_PACKET_DISCONNECT : public DEFAULT_PACKET
{
public:
	SC_PACKET_DISCONNECT()
	{
		size = sizeof( SC_PACKET_DISCONNECT );
		type = SC_DISCONNECT;
	};
};

class SC_PACKET_LOGIN_OK : public DEFAULT_PACKET
{
public:
	SC_PACKET_LOGIN_OK()
	{
		size = sizeof( SC_PACKET_LOGIN_OK );
		type = SC_LOGIN_OK;
	};
};

enum LOGIN_FAIL_REASON_ENUM
{
	LOGIN_FAIL_SAME_ID,
};

class SC_PACKET_LOGIN_FAIL : public DEFAULT_PACKET
{
public:
	SC_PACKET_LOGIN_FAIL( LOGIN_FAIL_REASON reason ) : reason( reason )
	{
		size = sizeof( SC_PACKET_LOGIN_FAIL );
		type = SC_LOGIN_FAIL;
	};
	LOGIN_FAIL_REASON reason; ///< 로그인 실패 이유.
};

class SC_PACKET_CHANGE_QUEUE : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_QUEUE( bool isEnq ) : enque( isEnq )
	{
		size = sizeof( SC_PACKET_CHANGE_QUEUE );
		type = SC_CHANGE_QUEUE;
	};
	bool enque{ false }; ///< 매칭 등록 여부.
};

class SC_PACKET_CHANGE_SCENE : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_SCENE( SCENETYPE sceneType ) : scene( sceneType )
	{
		size = sizeof( SC_PACKET_CHANGE_SCENE );
		type = SC_CHANGE_SCENE;
	};
	SCENETYPE scene; ///< 씬 상태.
};

class SC_PACKET_ATTACK : public DEFAULT_PACKET
{
public:
	SC_PACKET_ATTACK( UID uid ) : uid( uid )
	{
		size = sizeof( SC_PACKET_ATTACK );
		type = SC_ATTACK;
	};
	UID uid; ///< 식별자.
};

class SC_PACKET_UID : public DEFAULT_PACKET
{
public:
	SC_PACKET_UID( UID uid ) : uid( uid )
	{
		size = sizeof( SC_PACKET_UID );
		type = SC_UID;
	};
	UID uid; ///< 식별자.
};

class SC_PACKET_TIME : public DEFAULT_PACKET
{
public:
	SC_PACKET_TIME( TIME_TYPE time ) : time( time )
	{
		size = sizeof( SC_PACKET_TIME );
		type = SC_TIME;
	};
	TIME_TYPE time; ///< 시간값.
};

class SC_PACKET_SPAWN_CHARACTER : public DEFAULT_PACKET
{
public:
	SC_PACKET_SPAWN_CHARACTER( UID uid, CHARACTER_TYPE characterType, const std::string& userName, float x, float y ) : uid( uid ), characterType( characterType )
	{
		size = sizeof( SC_PACKET_SPAWN_CHARACTER );
		type = SC_SPAWN_CHARACTER;
		pos[ 0 ] = x;
		pos[ 1 ] = y;

		//오버플로우 방지
		assert( NAME_LENGTH >= userName.size() );
		memcpy( name, userName.c_str(), userName.size() );
	};
	UID            uid{};                     ///< 식별자
	CHARACTER_TYPE characterType{};           ///< 캐릭터 종류
	uint8_t        name[ NAME_LENGTH + 1 ]{}; ///< 이름
	float          pos[ 2 ]{};                ///< 위치
};

class SC_PACKET_DESTROY_CHARACTER : public DEFAULT_PACKET
{
public:
	SC_PACKET_DESTROY_CHARACTER( UID uid ) : uid( uid )
	{
		size = sizeof( SC_PACKET_DESTROY_CHARACTER );
		type = SC_DESTROY_CHARACTER;
	};
	UID uid{}; ///< 식별자
};

class SC_PACKET_CHANGE_HP : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_HP( UID uid, int hp, UID attacker = (UID)-1 ) : uid( uid ), attacker( attacker ), hp( hp )
	{
		size = sizeof( SC_PACKET_CHANGE_HP );
		type = SC_CHANGE_HP;
	};
	UID uid{};      ///< 식별자
	UID attacker{}; ///< 공격자 식별자
	int hp{};       ///< 체력
};

class SC_PACKET_CHANGE_LIFE : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_LIFE( UID uid, int8_t life ) : uid( uid ), life( life )
	{
		size = sizeof( SC_PACKET_CHANGE_LIFE );
		type = SC_CHANGE_LIFE;
	};
	UID    uid{}; ///< 식별자
	int8_t life;  ///< 남은 목숨
};

class SC_PACKET_CHARACTER_INFO : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHARACTER_INFO( UID uid, float x, float y, float z, float dx, float dy, bool teleport = false ) : uid( uid ), teleport( teleport )
	{
		size = sizeof( SC_PACKET_CHARACTER_INFO );
		type = SC_CHARACTER_INFO;
		pos[ 0 ] = x;
		pos[ 1 ] = y;
		pos[ 2 ] = z;
		dir[ 0 ] = dx;
		dir[ 1 ] = dy;
	};
	UID   uid{};             ///< 식별자
	float pos[ 3 ]{};        ///< 위치
	float dir[ 2 ]{};        ///< 방향
	bool  teleport{ false }; ///< 텔레포트 여부
};

class SC_PACKET_SPAWN_EFFECT : public DEFAULT_PACKET
{
public:
	SC_PACKET_SPAWN_EFFECT( UID uid, int eid, float x, float y, float z ) : uid( uid ), eid( eid )
	{
		size = sizeof( SC_PACKET_SPAWN_EFFECT );
		type = SC_SPAWN_EFFECT;
		pos[ 0 ] = x;
		pos[ 1 ] = y;
		pos[ 2 ] = z;
	};
	UID uid{};        ///< 식별자
	int eid{};        ///< 이펙트 식별자
	float pos[ 3 ]{}; ///< 위치
};

class SC_PACKET_USER_QUIT : public DEFAULT_PACKET
{
public:
	SC_PACKET_USER_QUIT( UID uid ) : uid( uid )
	{
		size = sizeof( SC_PACKET_USER_QUIT );
		type = SC_USER_QUIT;
	};
	UID uid{}; ///< 식별자
};

class SC_PACKET_SPAWN_BOMB : public DEFAULT_PACKET
{
public:
	SC_PACKET_SPAWN_BOMB( UID uid, float x, float y ) : uid( uid )
	{
		size = sizeof( SC_PACKET_SPAWN_BOMB );
		type = SC_SPAWN_BOMB;
		pos[ 0 ] = x;
		pos[ 1 ] = y;
	};
	UID uid{};        ///< 식별자 
	float pos[ 2 ]{}; ///< 위치
};

class CS_PACKET_REQUEST_LOGIN : public DEFAULT_PACKET
{
public:
	CS_PACKET_REQUEST_LOGIN( CHARACTER_TYPE charType, const std::string& userName ) : characterType( charType )
	{
		size = sizeof( CS_PACKET_REQUEST_LOGIN );
		type = CS_REQUEST_LOGIN;
		//오버플로우 방지
		assert( NAME_LENGTH >= userName.size() );
		memcpy( name, userName.c_str(), userName.size() );
	};
	CHARACTER_TYPE characterType{};    ///< 캐릭터 타입
	uint8_t name[ NAME_LENGTH + 1 ]{}; ///< 이름
};

class CS_PACKET_ATTACK : public DEFAULT_PACKET
{
public:
	CS_PACKET_ATTACK( UID uid ) : uid( uid )
	{
		size = sizeof( CS_PACKET_ATTACK );
		type = CS_ATTACK;
	};
	UID uid; ///< 식별자
};

class CS_PACKET_MOVEDIR : public DEFAULT_PACKET
{
public:
	CS_PACKET_MOVEDIR( UID uid, float dx, float dy ) : uid( uid ), dx( dx ), dy( dy )
	{
		size = sizeof( CS_PACKET_MOVEDIR );
		type = CS_MOVEDIR;
	};
	UID uid;  ///< 식별자
	float dx; ///< 방향
	float dy; ///< 방향
};

class CS_PACKET_ENQUEUE : public DEFAULT_PACKET
{
public:
	CS_PACKET_ENQUEUE()
	{
		size = sizeof( CS_PACKET_ENQUEUE );
		type = CS_ENQUEUE;
	};
};

class CS_PACKET_DEQUEUE : public DEFAULT_PACKET
{
public:
	CS_PACKET_DEQUEUE()
	{
		size = sizeof( CS_PACKET_DEQUEUE );
		type = CS_DEQUEUE;
	};
};

class CS_PACKET_READY : public DEFAULT_PACKET
{
public:
	CS_PACKET_READY( UID uid ) : uid( uid )
	{
		size = sizeof( CS_PACKET_READY );
		type = CS_READY;
	};
	UID uid; ///< 식별자
};

#pragma pack()