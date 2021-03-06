#include "DMRoom.h"
#include "..\Common\User.h"
#include "..\Utills\Logger.h"
#include "..\RoomManager.h"
#include "..\UserManager.h"
#include "..\MiniGameServer.h"


DMRoom::DMRoom()
{
	std::random_device rd;
	randomEngine.seed( rd() );
}

DMRoom::~DMRoom()
{
}

void DMRoom::Init()
{
	characterList.clear();
	userList.clear();
	eventData.Clear();
	infoData.Clear();
	leftTime   = DEFAULT_MATCH_TIME;
	readyCount = 0;
	serverID   = 0;
}

void DMRoom::ProcessJob( Job job )
{
	switch( job.first )
	{
		case CS_UPDATE:
			Update();
			break;

		case CS_LEAVEROOM:
			Disconnect( reinterpret_cast<User*>( job.second ) );
			break;

		case CS_ATTACK:
			//Logger::Log("유저 공격 수신 " + std::to_string(reinterpret_cast<UID>(job.second)));
			ProcessAttack( reinterpret_cast<UID>( job.second ) );
			break;

		case CS_MOVEDIR:
			//Logger::Log("유저 디렉션 수신 " + std::to_string(reinterpret_cast<MoveDirInfo*>(job.second)->uid));
			ProcessMoveDir( reinterpret_cast<MoveDirInfo*>( job.second ) );
			break;

		case CS_READY:
			ProcessReady( reinterpret_cast<UID>( job.second ) );
			break;

		default:
			Logger::Log( "처리되지 않은 룸 잡 발견" );
			break;
	}
}

void DMRoom::ProcessAttack( UID uid )
{
	if( EState::IDLE == characterList[ uid ]._playerInfo.curState
		|| EState::MOVE == characterList[ uid ]._playerInfo.curState )
	{
		// 기존에 날라가던걸 덮어씌우지는 않는가?
		// 대쉬 앵커
		characterList[ uid ]._hitColl._attackedPos = Vector3d(
			characterList[ uid ]._playerInfo.pos.x + characterList[ uid ]._playerInfo.dir.x * DASH_WEIGHT * characterList[ uid ]._playerInfo.moveSpeed,
			characterList[ uid ]._playerInfo.pos.y + characterList[ uid ]._playerInfo.dir.y * DASH_WEIGHT * characterList[ uid ]._playerInfo.moveSpeed,
			characterList[ uid ]._playerInfo.pos.z );
		characterList[ uid ]._hitColl._bAttacked = true;

		// 어택하면 일단 제자리
		characterList[ uid ]._playerInfo.dir.x = 0;
		characterList[ uid ]._playerInfo.dir.y = 0;

		characterList[ uid ]._playerInfo.curState = EState::ATTACK_READY;
		characterList[ uid ]._playerInfo.animTime = 0.0f;

		//공격패킷 중계
		SC_PACKET_ATTACK atkPacket{ uid };
		eventData.EmplaceBack( &atkPacket, atkPacket.size );
	}
}

void DMRoom::ProcessMoveDir( MoveDirInfo* info )
{
	if( nullptr == info ) return;
	if( characterList[ info->uid ]._playerInfo.curState == EState::ATTACK_READY ||
		characterList[ info->uid ]._hitColl._bAttacked == true ) return; // 어택상태이거나, 피격중이면 클라로부터 컨트롤러값 안받음

	Vector3d newDir{ info->x, info->y, 0.0f };
	characterList[ info->uid ]._playerInfo.dir = newDir.normalize();

	delete info;
}

void DMRoom::ProcessReady( UID uid )
{
	if( false == characterList[ uid ]._playerInfo.isReady )
	{
		characterList[ uid ]._playerInfo.isReady = true;
		if( ++readyCount == userList.size() )
		{
			//이후 업데이트 구문 추가
			//레디가 오기전에 유저 종료가 발생하면?
			//디스커넥 쪽에서도 동일 처리 필요
			Logger::Log( "모든 유저 준비 완료, 게임 시작" );
			lastUpdateTime = std::chrono::high_resolution_clock::now();
			MiniGameServer::Instance().AddEvent( queueType.second, EV_UPDATE, lastUpdateTime );
		};
	}
}

void DMRoom::UpdateLeftTime()
{
	float oldTime = leftTime;
	leftTime -= deltaTime;
	//1초단위로 클라/서버 간 남은 시간 동기화
	if( 1.0f <= oldTime )
	{
		TIME_TYPE oldSecTime = static_cast<TIME_TYPE>( oldTime );
		TIME_TYPE curSecTime = static_cast<TIME_TYPE>( leftTime );
		if( curSecTime != oldSecTime )
		{
			SC_PACKET_TIME timePacket{ curSecTime };
			eventData.EmplaceBack( &timePacket, timePacket.size );

			if( curSecTime % SUPER_BOMB_SPAWN_TIME == 0 ) // 4개 폭탄 소환
			{
				for( int i = 0; i < 4; i++ )
				{
					Logger::Log( "폭탄생성" );
					characterList.emplace_back( (UID)serverID, this );
					characterList[ serverID ]._playerInfo.pos = initialPos[ i ];
					characterList[ serverID ].SetAbility( 4 );
					characterList[ serverID ]._playerInfo.isBomb = true;

					SC_PACKET_SPAWN_BOMB spawnBombPacket{ serverID,initialPos[ i ].x, initialPos[ i ].y };
					eventData.EmplaceBack( &spawnBombPacket, spawnBombPacket.size );

					serverID++;
				}
			}
			else if( curSecTime % BOMB_SPAWN_TIME == 0 ) // 폭탄 소환
			{
				Logger::Log( "폭탄생성" );
				characterList.emplace_back( (UID)serverID, this );
				characterList[ serverID ]._playerInfo.pos = Vector3d{ 0,0,0 };
				characterList[ serverID ].SetAbility( 4 );
				characterList[ serverID ]._playerInfo.isBomb = true;

				SC_PACKET_SPAWN_BOMB spawnBombPacket{ serverID, 0, 0 };
				eventData.EmplaceBack( &spawnBombPacket, spawnBombPacket.size );
				serverID++;
			}

		}
	}
}

void DMRoom::UpdatePosition()
{
	for( auto& character : characterList )
		character.Update( deltaTime );
}

void DMRoom::UpdateCollider()
{
	// 공격 당함 체크 
	for( auto& chA : characterList ) // 공격하는 플레이어
	{
		if( chA.GetAttackCollider()._enabled == false ) continue; // 어택콜라이더 활성화 X -> return
		for( auto& chB : characterList )	// 맞는 플레이어
		{
			if( chA == chB ) continue;	// 내 자신은 공격 못한다.
			if( true == chB.IsInvincible() ) continue;
			if( chA._playerInfo.isBomb == true && chB._playerInfo.isBomb == true ) continue; // 둘 다 폭탄이면 충돌 체크 안함 
			if( chB._hitColl._enabled == false ) continue; // 맞는 캐릭터의 hitColl이 활성화 안되면 충돌 패스
			if( true == CheckCollider( chA.GetAttackCollider(), chB.GetHitCollider() ) ) // AttackColl, HitColl 충돌 체크
			{
				/* 피격체의 콜라이더를 피격당한상태로 바꾸고, 밀려날 위치를 부여한다. */
				chB._playerInfo.curState = EState::IDLE;		//문제 있음 -> 이때 공격패킷오면 바로 반격 가능
				chB.GetDamage( chA.id );

				/* 피격체가 밀려나갈 방향 구하기 */
				Vector3d disVec = chB._playerInfo.pos - chA._playerInfo.pos;

				//위치가 동일하다면 랜덤한 위치로 튀도록 벡터 난수생성.
				while( true == disVec.isZero() )
				{
					disVec.x = randomRange( randomEngine );
					disVec.y = randomRange( randomEngine );
				}
				disVec = disVec.normalize();

				//넉백 위치 부여
				chB.GetHitCollider()._bAttacked = true;

				//스택버전
				chB.GetHitCollider()._attackedPos = Vector3d(
					chB._playerInfo.pos.x + ( chB._playerInfo.knockbackWeight * ( chB._playerInfo.hitPoint ) * chA._playerInfo.attackPower * disVec.x ),
					chB._playerInfo.pos.y + ( chB._playerInfo.knockbackWeight * ( chB._playerInfo.hitPoint ) * chA._playerInfo.attackPower * disVec.y ),
					chB._playerInfo.pos.z
				);

				// 이펙트 소환 패킷 중계
				SC_PACKET_SPAWN_EFFECT effectPacket{ 0, (int)EObjectType::HitEffect, chB._playerInfo.pos.x, chB._playerInfo.pos.y, chB._playerInfo.pos.z };
				eventData.EmplaceBack( &effectPacket, effectPacket.size );
			}
		}
		chA.GetAttackCollider()._enabled = false;
	}

	for( auto& ch : characterList ) // 공격하는 플레이어
	{
		if( EState::FALL == ch.GetCurState()
			|| EState::DIE == ch.GetCurState() )
			continue;

		//맵 밖으로 벗어나면 사망처리
		if( false == CheckCollider( mapCollider, ch.GetHitCollider() ) )
		{
			ch._playerInfo.curState = EState::FALL;
			ch.GetAttackCollider()._enabled = false;
		}
	}
}

bool DMRoom::CheckCollider( const Collider& a, const Collider& b )
{
	if( a.GetMaxX() < b.GetMinX() || a.GetMinX() > b.GetMaxX() ) return false;
	if( a.GetMaxY() < b.GetMinY() || a.GetMinY() > b.GetMaxY() ) return false;
	return true;
}

void DMRoom::Update()
{
	currentUpdateTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>( currentUpdateTime - lastUpdateTime ).count();

	GameLogic();
	SendGameState();
	isEnd = EndCheck();
	if( true == isEnd )
	{
		End();
		Logger::Log( "매치 종료, 룸 매니저에 삭제 요청" );
	}
	else
	{
		MiniGameServer::Instance().AddEvent( queueType.second, EV_UPDATE, lastUpdateTime + std::chrono::milliseconds( UPDATE_INTERVAL ) );
	}
	lastUpdateTime = currentUpdateTime;
}

void DMRoom::SetQueueType( QueueType queType )
{
	queueType = queType;
}

void DMRoom::GameLogic()
{
	UpdateLeftTime();
	UpdatePosition();
	UpdateCollider();
}

void DMRoom::SendGameState()
{
	//Send data to clients.
	for( size_t i = 0; i < characterList.size(); ++i )
	{
		SC_PACKET_CHARACTER_INFO infoPacket{ (UID)i,
			characterList[ i ]._playerInfo.pos.x, characterList[ i ]._playerInfo.pos.y, characterList[ i ]._playerInfo.pos.z,
			characterList[ i ]._playerInfo.dir.x, characterList[ i ]._playerInfo.dir.y };
		infoData.EmplaceBack( &infoPacket, infoPacket.size );
	}

	//이벤트 데이터 뒷부분에 통합.
	eventData.EmplaceBack( infoData.data, infoData.len );
	for( auto& user : userList )
		MiniGameServer::Instance().SendPacket( user, eventData.data, eventData.len );

	//이후 초기화
	eventData.Clear();
	infoData.Clear();
}

void DMRoom::Regist( std::vector<User*> users )
{
	//유저 등록 처리, UID도 전송.
	for( size_t i = 0; i < users.size(); ++i )
	{
		if( nullptr != users[ i ] )
		{
			users[ i ]->roomPtr = this;
			userList.emplace_back( users[ i ] );
			characterList.emplace_back( (UID)i, this );

			characterList[ i ].userPtr = users[ i ];
			characterList[ i ]._playerInfo.initialPos = initialPos[ i % _countof( initialPos ) ];
			characterList[ i ]._playerInfo.pos = characterList[ i ]._playerInfo.initialPos;
			characterList[ i ].SetAbility( users[ i ]->characterType );

			SC_PACKET_UID packet{ (UID)i };
			MiniGameServer::Instance().SendPacket( users[ i ], &packet );

			SC_PACKET_SPAWN_CHARACTER spawnCharacterPacket{ (UID)i, users[ i ]->characterType, users[ i ]->id, characterList[ i ]._playerInfo.initialPos.x, characterList[ i ]._playerInfo.initialPos.y };
			eventData.EmplaceBack( &spawnCharacterPacket, spawnCharacterPacket.size );
			serverID++;
		}
	}

	// [스폰데이터 | 게임 시작 시그널] 전송
	SC_PACKET_CHANGE_SCENE changeScenePacket{ SCENE_GAME };
	eventData.EmplaceBack( &changeScenePacket, changeScenePacket.size );
	SC_PACKET_TIME timePacket{ (TIME_TYPE)DEFAULT_MATCH_TIME };
	eventData.EmplaceBack( &timePacket, timePacket.size );

	for( auto& user : userList )
		MiniGameServer::Instance().SendPacket( user, eventData.data, eventData.len );
	eventData.Clear();
	return;
}

void DMRoom::Disconnect( User* user )
{
	//유저를 룸에서 삭제 및 유저 매니저에 통보.
	//실패시는 무통보.
	if( nullptr != user )
	{
		auto iter = std::find( userList.begin(), userList.end(), user );
		if( iter != userList.end() )
		{
			UID uid{ (UID)-1 };
			for( auto& ch : characterList )
			{
				if( user == ch.userPtr )
				{
					uid = ch.id;
					ch.userPtr = nullptr;
					break;
				}
			}
			if( -1 == uid ) return;

			//게임 시작전 상태라면 레디처리.
			ProcessReady( uid );

			//유저 삭제처리
			user->roomPtr = nullptr;
			userList.erase( iter );
			UserManager::Instance().PushJob( USER_LEAVEROOM, reinterpret_cast<void*>( user->uid ) );

			//유저 종료 통보
			SC_PACKET_USER_QUIT quitPacket{ uid };
			eventData.EmplaceBack( &quitPacket, quitPacket.size );
		}
	}
}

void DMRoom::End()
{
	//유저 종료처리 및 룸 매니저에 통보
	//마지막 스코어처리도 이곳에서?
	QuitAllUser();
	RoomManager::Instance().PushJob( RMGR_DESTROY, reinterpret_cast<void*>( queueType.second ) );
}

bool DMRoom::EndCheck()
{
	int leftUserNum = 0;
	for( auto& ch : characterList )
		if( true == ch.IsAlive() )
			++leftUserNum;

	if( 1 == leftUserNum )
		return true;

	//제한시간이 다 소모되면 게임 종료
	if( 0 >= leftTime )
	{
		Logger::Log( "룸 타임아웃, 매치 종료" );
		return true;
	}

	//유저가 모두 접속을 종료하면 게임 종료
	if( true == userList.empty() )
	{
		Logger::Log( "남은 유저 수 0, 매치 종료" );
		return true;
	}

	return false;
}

void DMRoom::QuitAllUser()
{
	//벡터인데 앞부분부터 삭제함. 일단 있는 함수를 활용하지만 이후 개선 필요
	SC_PACKET_CHANGE_SCENE changeScenePacket{ SCENE_MAIN };
	for( auto& user : userList )
		MiniGameServer::Instance().SendPacket( user, &changeScenePacket, changeScenePacket.size );
	while( false == userList.empty() )
		Disconnect( userList.front() );
}