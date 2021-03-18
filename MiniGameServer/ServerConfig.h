#pragma once
#include "Utills/Vector3d.h"

//Server
constexpr auto RECV_BUF_SIZE = 1024;									//수신버퍼 크기
constexpr size_t THREAD_NUM = 10;										//WorkerThread 수
constexpr short SERVER_PORT = 15600;									//서버 포트

constexpr size_t REQUIRE_USER_NUM = 2;									//한 매치당 필요한 인원 수
constexpr size_t MAX_USER_SIZE = 1000;									//서버에 접속할 수 있는 유저 수
constexpr size_t MAX_ROOM_SIZE = MAX_USER_SIZE / REQUIRE_USER_NUM + 1;	//서버에 미리 만들어둘 방의 수

//DMRooms
constexpr long long UPDATE_INTERVAL = 20;			//업데이트 간격
constexpr float DEFAULT_MATCH_TIME = 180.0f;		//매치 시간
constexpr float MAP_WIDTH = 1000.0f;				//맵 가로크기
constexpr float MAP_HEIGHT = 1000.0f;				//맵 세로크기

//Characters

constexpr float CHARACTER_HITBOX_WIDTH = 100.0f;			//캐릭터 히트박스 가로
constexpr float CHARACTER_HITBOX_HEIGHT = 100.0f;			//캐릭터 히트박스 세로
constexpr float ATTACK_HITBOX_WIDTH = 200.0f;				//캐릭터 공격 히트박스 가로
constexpr float ATTACK_HITBOX_HEIGHT = 100.0f;				//캐릭터 공격 히트박스 세로
constexpr float CHARACTER_DROP_SPEED = 100.0f;				//캐릭터 기본 낙하속도
constexpr float CHARACTER_MOVE_SPEED = 1000.0f;				//캐릭터 기본 이동속도
constexpr float CHARACTER_KNOCKBACK_WEIGHT = 1.0f;			//캐릭터 넉백 가중치
constexpr float CHARACTER_ATTACK_POWER = 200.0f;			//캐릭터 공격력
constexpr float INVINCIBLE_TIME = 1.0f;

constexpr char CHARACTER_LIFE = 3;							//캐릭터 목숨
constexpr int CHARACTER_MAX_HP = 3;							//캐릭터 생명력

//공식 1 / fps * animation frame
constexpr float ATK_READY_TIME = 0.1333333f;		//공격준비 프레임은 15fps 기준으로 2프레임
constexpr float ATK_TIME = 0.3333333f;				//공격 프레임은 15fps 기준으로 5프레임
constexpr float DROP_SPEED = 5000.0f;				//중력
constexpr float DEATH_HEIGHT = -1000.0f;			//죽는 높이
constexpr float RESPAWN_TIME = 0.0f;				//리스폰 시간
constexpr float DASH_WEIGHT = 0.5f;

constexpr float WAIT_RESPAWN_SPACE = 5555.5f;	//캐릭터가 죽었을 때 잠깐 숨길 공간의 좌표(언리얼 KILLZ에 포함되면 안됨)