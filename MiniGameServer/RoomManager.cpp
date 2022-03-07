#include "RoomManager.h"
#include "Utills/Logger.h"
#include "MiniGameServer.h"

RoomManager& RoomManager::Instance()
{
	static RoomManager* roomManager = new RoomManager();
	return *roomManager;
}

DMRoom* RoomManager::GetRoom( size_t idx )
{
	return &m_roomList[ idx ];
}

void RoomManager::ProcessJob( Job job )
{
	switch( job.first )
	{
		case RMGR_CREATE:
			ProcessCreateRoom( reinterpret_cast<CreateRoomInfo*>( job.second ) );
			break;

		case RMGR_DESTROY:
			ProcessDestroyRoom( reinterpret_cast<size_t>( job.second ) );
			break;

		default:
			Logger::Log( "처리되지 않은 룸 매니저 잡 발견" );
			break;
	}
}

RoomManager::RoomManager()
{
	queueType = { static_cast<size_t>( GlobalQueueType::ROOM_MANAGER ), 0 };
	//인덱스 풀 초기화.
	for( size_t idx = 1; MAX_ROOM_SIZE >= idx; ++idx )
		m_indexPool.push( MAX_ROOM_SIZE - idx );

	for( size_t i = 0; i < MAX_ROOM_SIZE; ++i )
		m_roomList[ i ].SetQueueType( { static_cast<size_t>( GlobalQueueType::ROOM ), i } );
}

void RoomManager::ProcessCreateRoom( CreateRoomInfo* info )
{
	if( nullptr == info )
	{
		Logger::Log( "잘못된 방생성 - NULL 시도 발생" );
		return;
	}

	if( true == m_indexPool.empty() )
	{
		//할당할 수 있는 방이 없다. -> 받지말자.
		Logger::Log( "할당할 수 있는 방이 없음" );
		return;
	}

	size_t roomIdx = m_indexPool.top();
	m_indexPool.pop();

	m_roomList[ roomIdx ].Init();
	m_roomList[ roomIdx ].Regist( info->users );

	Logger::Log( "룸 매니저에서 방 생성 완료" );
	delete info;
}

void RoomManager::ProcessDestroyRoom( size_t idx )
{
	if( false == m_roomList[ idx ].IsEnd() ) return;

	m_indexPool.push( idx );
	Logger::Log( "룸 매니저에서 방 삭제 완료" );
}
