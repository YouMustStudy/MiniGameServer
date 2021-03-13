#pragma once

//Server -> Client.
enum SC_PACKET
{
	//Main
	SC_CONNECT_OK,				///< ���� ����
	SC_LOGIN_OK,				///< �α��� ����
	SC_LOGIN_FAIL,				///< �α��� ����
	SC_CHANGE_QUEUE,			///< ��ġť ������ ����
	SC_CHANGE_SCENE,			///< ���ӽ���, ���Ḧ �˸�

	//Game
	SC_UID,						///< �������� ���� UID ����
	SC_TIME,					///< ����/Ŭ�� �ð� ����ȭ
	SC_SPAWN_CHARACTER,			///< ĳ���� ������Ʈ �߰�
	SC_DESTROY_CHARACTER,		///< ĳ���� ������Ʈ ����
	SC_ATTACK,					///< ĳ���� ����
	SC_GET_ITEM,				///< ĳ���� ������ ȹ��
	SC_CHANGE_HP,				///< ĳ���� ü�� ����
	SC_CHANGE_SCORE,			///< ĳ���� ���ھ� ����
	SC_CHARACTER_INFO,			///< ĳ������ ��ġ, ���� �� ��
	SC_SPAWN_EFFECT,			///< Ÿ�ݸ�� �� ����Ʈ ���
	SC_COUNT
};

//Client -> Server.
enum CS_PACKET
{
	//To UserManager
	CS_REQUEST_LOGIN,			///< �α��� ��û
	CS_ENQUEUE,					///< ��ġť ��� ��û
	CS_DEQUEUE,					///< ��ġť ������� ��û

	//To Room
	CS_UPDATE,					///< �� ������Ʈ ��û
	CS_LEAVEROOM,				///< ���� ���� ���� ��û
	CS_ATTACK,					///< ���� ���ݿ�û
	CS_MOVEDIR,					///< ���� ������ ��û
	CS_COUNT
};

enum SCENE_TYPE
{
	SCENE_MAIN,
	SCENE_GAME
};

using PACKET_TYPE = unsigned char;
using PACKET_SIZE = unsigned short;
using UID = size_t;
using SCENETYPE = unsigned char;
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

using LOGIN_FAIL_REASON = unsigned char;
enum LOGIN_FAIL_REASON_ENUM
{
	LOGIN_FAIL_SAME_ID,
};

class SC_PACKET_LOGIN_FAIL : public DEFAULT_PACKET
{
public:
	SC_PACKET_LOGIN_FAIL(LOGIN_FAIL_REASON reason) : reason(reason)
	{
		size = sizeof(SC_PACKET_LOGIN_FAIL);
		type = SC_LOGIN_FAIL;
	};
	LOGIN_FAIL_REASON reason;
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

class SC_PACKET_CHANGE_SCENE : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_SCENE(SCENETYPE sceneType) : scene(sceneType)
	{
		size = sizeof(SC_PACKET_CHANGE_SCENE);
		type = SC_CHANGE_SCENE;
	};
	SCENETYPE scene;
};

class SC_PACKET_ATTACK : public DEFAULT_PACKET
{
public:
	SC_PACKET_ATTACK(UID uid) : uid(uid)
	{
		size = sizeof(SC_PACKET_ATTACK);
		type = SC_ATTACK;
	};
	UID uid;
};

class SC_PACKET_UID : public DEFAULT_PACKET
{
public:
	SC_PACKET_UID(UID uid) : uid(uid)
	{
		size = sizeof(SC_PACKET_UID);
		type = SC_UID;
	};
	UID uid;
};

class SC_PACKET_TIME : public DEFAULT_PACKET
{
public:
	SC_PACKET_TIME(unsigned int time) : time(time)
	{
		size = sizeof(SC_PACKET_TIME);
		type = SC_TIME;
	};
	unsigned int time;
};

class SC_PACKET_SPAWN_CHARACTER : public DEFAULT_PACKET
{
public:
	SC_PACKET_SPAWN_CHARACTER(UID uid, int characterType, float x, float y) : uid(uid), characterType(characterType)
	{
		size = sizeof(SC_PACKET_SPAWN_CHARACTER);
		type = SC_SPAWN_CHARACTER;
		pos[0] = x;
		pos[1] = y;
	};
	UID uid{};
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
	UID uid{};
};

class SC_PACKET_GET_ITEM : public DEFAULT_PACKET
{
public:
	SC_PACKET_GET_ITEM(UID uid, int item) : uid(uid), item(item)
	{
		size = sizeof(SC_PACKET_GET_ITEM);
		type = SC_GET_ITEM;
	};
	UID uid{};
	int item{};
};

class SC_PACKET_CHANGE_HP : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_HP(UID uid, int hp) : uid(uid), hp(hp)
	{
		size = sizeof(SC_PACKET_CHANGE_HP);
		type = SC_CHANGE_HP;
	};
	UID uid{};
	int hp{};
};

class SC_PACKET_CHANGE_SCORE : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHANGE_SCORE(UID uid, int curKill, int totalKill) : uid(uid), curKill(curKill), totalKill(totalKill)
	{
		size = sizeof(SC_PACKET_CHANGE_SCORE);
		type = SC_CHANGE_SCORE;
	};
	UID uid{};
	int curKill{};
	int totalKill{};
};

class SC_PACKET_CHARACTER_INFO : public DEFAULT_PACKET
{
public:
	SC_PACKET_CHARACTER_INFO(UID uid, float x, float y, float z, float dx, float dy) : uid(uid)
	{
		size = sizeof(SC_PACKET_CHARACTER_INFO);
		type = SC_CHARACTER_INFO;
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
		dir[0] = dx;
		dir[1] = dy;
	};
	UID uid{};
	float pos[3]{};
	float dir[2]{};
};

class SC_PACKET_SPAWN_EFFECT : public DEFAULT_PACKET
{
public:
	SC_PACKET_SPAWN_EFFECT(UID uid, int eid, float x, float y, float z) : uid(uid), eid(eid)
	{
		size = sizeof(SC_PACKET_SPAWN_EFFECT);
		type = SC_SPAWN_EFFECT;
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	};
	UID uid{};
	int eid{};
	float pos[3]{};
};

class CS_PACKET_REQUEST_LOGIN : public DEFAULT_PACKET
{
public:
	CS_PACKET_REQUEST_LOGIN()
	{
		size = sizeof(CS_PACKET_REQUEST_LOGIN);
		type = CS_REQUEST_LOGIN;
	};
	wchar_t name[NAME_LENGTH + 1]{};
};

class CS_PACKET_ATTACK : public DEFAULT_PACKET
{
public:
	CS_PACKET_ATTACK(UID uid) : uid(uid)
	{
		size = sizeof(CS_PACKET_ATTACK);
		type = CS_ATTACK;
	};
	UID uid;
};

class CS_PACKET_MOVEDIR : public DEFAULT_PACKET
{
public:
	CS_PACKET_MOVEDIR(UID uid, float dx, float dy) : uid(uid), dx(dx), dy(dy)
	{
		size = sizeof(CS_PACKET_MOVEDIR);
		type = CS_MOVEDIR;
	};
	UID uid;
	float dx;
	float dy;
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