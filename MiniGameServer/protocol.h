#pragma once

//Server -> Client.
enum SC_PACKET
{
	SC_CONNECT_OK,				///< 연결 성공
	SC_LOGIN_OK,				///< 로그인 성공
	SC_LOGIN_FAIL,				///< 로그인 실패
	SC_CHANGE_QUEUE,			///< 매치 진입상태 변경
	SC_SPAWN_CHARACTER,			///< 캐릭터 오브젝트 추가
	SC_DESTROY_CHARACTER,		///< 캐릭터 오브젝트 삭제
	SC_GET_ITEM,				///< 캐릭터 아이템 획득
	SC_CHANGE_HP,				///< 캐릭터 체력 변경
	SC_CHANGE_SCORE,			///< 캐릭터 스코어 변경
	SC_CHARACTER_INFO,			///< 캐릭터의 위치, 방향 등 값
	SC_SPAWN_EFFECT,			///< 타격모션 등 이펙트 출력
	SC_COUNT
};

//Client -> Server.
enum CS_PACKET
{
	//To UserManager
	CS_REQEUST_LOGIN,			///< 로그인 요청
	CS_ENQUEUE,					///< 매치큐 등록 요청
	CS_DEQUEUE,					///< 매치큐 등록해제 요청

	//To Room
	CS_UPDATE,					///< 방 업데이트 요청
	CS_KEYUP,					///< 키입력(띔)
	CS_KEYDOWN,					///< 키입력(눌림)
	CS_COUNT
};

using PACKET_TYPE = unsigned char;
using PACKET_SIZE = unsigned short;
constexpr size_t NAME_LENGTH = 20;

#pragma pack(1)
class DEFAULT_PACKET
{
public:
	PACKET_SIZE size{0};
	PACKET_TYPE type{0};
};

class SC_PACKET_CONNECT_OK : public DEFAULT_PACKET
{
public:
	SC_PACKET_CONNECT_OK()
	{
		size = sizeof(SC_PACKET_CONNECT_OK);
		type = SC_CONNECT_OK;
	};
};

class SC_PACKET_LOGIN_OK : public DEFAULT_PACKET
{
public:
	SC_PACKET_LOGIN_OK()
	{
		size = sizeof(SC_PACKET_LOGIN_OK); 
		type = SC_LOGIN_OK;
	};
};

class SC_PACKET_LOGIN_FAIL : public DEFAULT_PACKET
{
public:
	SC_PACKET_LOGIN_FAIL()
	{
		size = sizeof(SC_PACKET_LOGIN_FAIL);
		type = SC_LOGIN_FAIL;
	};
};

class SC_PACKET_CHANGE_QUEUE : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_QUEUE(bool isEnq) : enque(isEnq)
	{
		size = sizeof(SC_PACKET_CHANGE_QUEUE);
		type = SC_CHANGE_QUEUE;
	};
	bool enque{ false };
};

class SC_PACKET_SPAWN_CHARACTER : public DEFAULT_PACKET
{
public:
	SC_PACKET_SPAWN_CHARACTER(int uid, int characterType, float x, float y) : uid(uid), characterType(characterType)
	{
		size = sizeof(SC_PACKET_SPAWN_CHARACTER);
		type = SC_SPAWN_CHARACTER;
		pos[0] = x;
		pos[1] = y;
	};
	int uid{};
	int characterType{};
	wchar_t name[NAME_LENGTH + 1]{};
	float pos[2]{};
};

class SC_PACKET_DESTROY_CHARACTER : public DEFAULT_PACKET
{
public:
	SC_PACKET_DESTROY_CHARACTER(int uid) : uid(uid)
	{
		size = sizeof(SC_PACKET_DESTROY_CHARACTER);
		type = SC_DESTROY_CHARACTER;
	};
	int uid{};
};

class SC_PACKET_GET_ITEM : public DEFAULT_PACKET
{
public:
	SC_PACKET_GET_ITEM(int uid, int item) : uid(uid), item(item)
	{
		size = sizeof(SC_PACKET_GET_ITEM);
		type = SC_GET_ITEM;
	};
	int uid{};
	int item{};
};

class SC_PACKET_CHANGE_HP : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_HP(int uid, int hp) : uid(uid), hp(hp)
	{
		size = sizeof(SC_PACKET_CHANGE_HP);
		type = SC_CHANGE_HP;
	};
	int uid{};
	int hp{};
};

class SC_PACKET_CHANGE_SCORE : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_SCORE(int uid, int curKill, int totalKill) : uid(uid), curKill(curKill), totalKill(totalKill)
	{
		size = sizeof(SC_PACKET_CHANGE_SCORE);
		type = SC_CHANGE_SCORE;
	};
	int uid{};
	int curKill{};
	int totalKill{};
};

class SC_PACKET_CHARACTER_INFO : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHARACTER_INFO(int uid, float x, float y, float dx, float dy) : uid(uid)
	{
		size = sizeof(SC_PACKET_CHARACTER_INFO);
		type = SC_CHARACTER_INFO;
		pos[0] = x;
		pos[1] = y;
		dir[0] = dx;
		dir[1] = dy;
	};
	int uid{};
	float pos[2]{};
	float dir[2]{};
	char animIndex{0};
};

class SC_PACKET_SPAWN_EFFECT : public DEFAULT_PACKET
{
public:
	SC_PACKET_SPAWN_EFFECT(int uid, int eid) : uid(uid), eid(eid)
	{
		size = sizeof(SC_PACKET_SPAWN_EFFECT);
		type = SC_SPAWN_EFFECT;
	};
	int uid{};
	int eid{};
};

class CS_PACKET_REQEUST_LOGIN : public DEFAULT_PACKET
{
public:
	CS_PACKET_REQEUST_LOGIN()
	{
		size = sizeof(CS_PACKET_REQEUST_LOGIN);
		type = CS_REQEUST_LOGIN;
	};
	wchar_t name[NAME_LENGTH + 1]{};
};

class CS_PACKET_KEYUP : public DEFAULT_PACKET
{
public:
	CS_PACKET_KEYUP(char key) : key(key)
	{
		size = sizeof(CS_PACKET_KEYUP);
		type = CS_KEYUP;
	};
	char key;
};

class CS_PACKET_KEYDOWN : public DEFAULT_PACKET
{
public:
	CS_PACKET_KEYDOWN(char key) : key(key)
	{
		size = sizeof(CS_PACKET_KEYDOWN);
		type = CS_KEYDOWN;
	};
	char key;
};

class CS_PACKET_ENQUEUE : public DEFAULT_PACKET
{
public:
	CS_PACKET_ENQUEUE()
	{
		size = sizeof(CS_PACKET_ENQUEUE);
		type = CS_ENQUEUE;
	};
};

class CS_PACKET_DEQUEUE : public DEFAULT_PACKET
{
public:
	CS_PACKET_DEQUEUE()
	{
		size = sizeof(CS_PACKET_DEQUEUE);
		type = CS_DEQUEUE;
	};
};

#pragma pack()