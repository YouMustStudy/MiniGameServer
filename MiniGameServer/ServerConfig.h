#pragma once
#include "Utills/Vector3d.h"

//Server
constexpr auto RECV_BUF_SIZE = 1024;									//���Ź��� ũ��
constexpr size_t THREAD_NUM = 10;										//WorkerThread ��
constexpr short SERVER_PORT = 15600;									//���� ��Ʈ

constexpr size_t REQUIRE_USER_NUM = 2;									//�� ��ġ�� �ʿ��� �ο� ��
constexpr size_t MAX_USER_SIZE = 1000;									//������ ������ �� �ִ� ���� ��
constexpr size_t MAX_ROOM_SIZE = MAX_USER_SIZE / REQUIRE_USER_NUM + 1;	//������ �̸� ������ ���� ��

//DMRooms
constexpr long long UPDATE_INTERVAL = 20;			//������Ʈ ����
constexpr float DEFAULT_MATCH_TIME = 180.0f;		//��ġ �ð�
constexpr float MAP_WIDTH = 1000.0f;				//�� ����ũ��
constexpr float MAP_HEIGHT = 1000.0f;				//�� ����ũ��

//Characters

constexpr float CHARACTER_HITBOX_WIDTH = 100.0f;			//ĳ���� ��Ʈ�ڽ� ����
constexpr float CHARACTER_HITBOX_HEIGHT = 100.0f;			//ĳ���� ��Ʈ�ڽ� ����
constexpr float ATTACK_HITBOX_WIDTH = 200.0f;				//ĳ���� ���� ��Ʈ�ڽ� ����
constexpr float ATTACK_HITBOX_HEIGHT = 100.0f;				//ĳ���� ���� ��Ʈ�ڽ� ����
constexpr float CHARACTER_DROP_SPEED = 100.0f;				//ĳ���� �⺻ ���ϼӵ�
constexpr float CHARACTER_MOVE_SPEED = 1000.0f;				//ĳ���� �⺻ �̵��ӵ�
constexpr float CHARACTER_KNOCKBACK_WEIGHT = 1.0f;			//ĳ���� �˹� ����ġ
constexpr float CHARACTER_ATTACK_POWER = 200.0f;			//ĳ���� ���ݷ�
constexpr float INVINCIBLE_TIME = 1.0f;

constexpr char CHARACTER_LIFE = 3;							//ĳ���� ���
constexpr int CHARACTER_MAX_HP = 3;							//ĳ���� �����

//���� 1 / fps * animation frame
constexpr float ATK_READY_TIME = 0.1333333f;		//�����غ� �������� 15fps �������� 2������
constexpr float ATK_TIME = 0.3333333f;				//���� �������� 15fps �������� 5������
constexpr float DROP_SPEED = 5000.0f;				//�߷�
constexpr float DEATH_HEIGHT = -1000.0f;			//�״� ����
constexpr float RESPAWN_TIME = 0.0f;				//������ �ð�
constexpr float DASH_WEIGHT = 0.5f;

constexpr float WAIT_RESPAWN_SPACE = 5555.5f;	//ĳ���Ͱ� �׾��� �� ��� ���� ������ ��ǥ(�𸮾� KILLZ�� ���ԵǸ� �ȵ�)