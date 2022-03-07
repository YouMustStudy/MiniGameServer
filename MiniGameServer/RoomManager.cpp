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
			Logger::Log( "ó������ ���� �� �Ŵ��� �� �߰�" );
			break;
	}
}

RoomManager::RoomManager()
{
	queueType = { static_cast<size_t>( GlobalQueueType::ROOM_MANAGER ), 0 };
	//�ε��� Ǯ �ʱ�ȭ.
	for( size_t idx = 1; MAX_ROOM_SIZE >= idx; ++idx )
		m_indexPool.push( MAX_ROOM_SIZE - idx );

	for( size_t i = 0; i < MAX_ROOM_SIZE; ++i )
		m_roomList[ i ].SetQueueType( { static_cast<size_t>( GlobalQueueType::ROOM ), i } );
}

void RoomManager::ProcessCreateRoom( CreateRoomInfo* info )
{
	if( nullptr == info )
	{
		Logger::Log( "�߸��� ����� - NULL �õ� �߻�" );
		return;
	}

	if( true == m_indexPool.empty() )
	{
		//�Ҵ��� �� �ִ� ���� ����. -> ��������.
		Logger::Log( "�Ҵ��� �� �ִ� ���� ����" );
		return;
	}

	size_t roomIdx = m_indexPool.top();
	m_indexPool.pop();

	m_roomList[ roomIdx ].Init();
	m_roomList[ roomIdx ].Regist( info->users );

	Logger::Log( "�� �Ŵ������� �� ���� �Ϸ�" );
	delete info;
}

void RoomManager::ProcessDestroyRoom( size_t idx )
{
	if( false == m_roomList[ idx ].IsEnd() ) return;

	m_indexPool.push( idx );
	Logger::Log( "�� �Ŵ������� �� ���� �Ϸ�" );
}
