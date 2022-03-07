#include "UserManager.h"
#include "RoomManager.h"
#include "MiniGameServer.h"


UserManager& UserManager::Instance()
{
	static UserManager* userManager = new UserManager();
	return *userManager;
}

void UserManager::SetRequireUserNum( size_t num )
{
	m_matchQueue.SetRequireUserNum( num );
}

void UserManager::ProcessJob( Job job )
{
	switch( job.first )
	{
		case USER_ACCEPT:
			ProcessAccept( reinterpret_cast<AcceptInfo*>( job.second ) );
			break;

		case USER_DISCONN:
			ProcessDisconnect( reinterpret_cast<size_t>( job.second ) );
			break;

		case USER_LOGIN:
			ProcessLogin( reinterpret_cast<LoginInfo*>( job.second ) );
			break;

		case USER_ENQUEUE:
			ProcessEnqueue( reinterpret_cast<size_t>( job.second ) );
			break;

		case USER_DEQUEUE:
			ProcessDequeue( reinterpret_cast<size_t>( job.second ) );
			break;

		case USER_LEAVEROOM:
			ProcessLeaveRoom( reinterpret_cast<size_t>( job.second ) );
			break;

		default:
			Logger::Log( "ó������ ���� ���� �Ŵ��� �� �߰�" );
			break;
	}
}

UserManager::UserManager()
{
	queueType = { static_cast<size_t>( GlobalQueueType::USER_MANAGER ), 0 };
	//�ε��� Ǯ �ʱ�ȭ.
	for( size_t idx = 1; MAX_USER_SIZE >= idx; ++idx )
		m_indexPool.push( MAX_USER_SIZE - idx );
	//���� �ʱ�ȭ
	for( size_t idx = 0; MAX_USER_SIZE > idx; ++idx )
		m_userList[ idx ].uid = (UID)idx;
}

void UserManager::ProcessAccept( AcceptInfo* info )
{
	if( nullptr == info )
	{
		Logger::Log( "�߸��� ACCEPT - NULL �õ� �߻�" );
		return;
	}

	if( true == m_indexPool.empty() )
	{
		//�Ҵ��� �� �ִ� ������ ����. -> ��������.
		Logger::Log( "�Ҵ��� �� �ִ� ������ ����" );
		closesocket( info->socket );
		return;
	}

	size_t userIdx = m_indexPool.top();
	m_indexPool.pop();

	if( ST_DISCONN != m_userList[ userIdx ].state )
	{
		Logger::Log( "������ �� ���� ���� �Ҵ�" );
		return;
	}

	m_userList[ userIdx ].socket = info->socket;
	m_userList[ userIdx ].addr = info->addr;
	m_userList[ userIdx ].state = ST_NOLOGIN;
	m_userList[ userIdx ].recvOver.Init( RECV_BUF_SIZE );
	m_userList[ userIdx ].recvOver.SetEvent( EV_RECV );

	CreateIoCompletionPort( reinterpret_cast<HANDLE>( m_userList[ userIdx ].socket ), m_workerIOCP, userIdx, 0 );
	m_userList[ userIdx ].SetRecv();

	SC_PACKET_CONNECT_OK packet;
	MiniGameServer::Instance().SendPacket( &m_userList[ userIdx ], &packet );

	char nameBuf[ INET_ADDRSTRLEN ]{ 0, };
	inet_ntop( AF_INET, &m_userList[ userIdx ].addr.sin_addr, nameBuf, INET_ADDRSTRLEN );
	Logger::Log( "���� ���� : " + std::string( nameBuf ) + ":" + std::to_string( m_userList[ userIdx ].addr.sin_port ) );

	delete info;
}

void UserManager::ProcessDisconnect( size_t idx )
{
	if( ST_QUEUE == m_userList[ idx ].state )
		m_matchQueue.Dequeue( idx );

	//���� ���� ��ȯ ��
	m_userList[ idx ].state = ST_DISCONN;

	//�濡 ������ �� ������ ó��.
	if( nullptr != m_userList[ idx ].roomPtr )
		m_userList[ idx ].roomPtr->PushJob( CS_LEAVEROOM, reinterpret_cast<void*>( &m_userList[ idx ] ) );
	else
		DisconnectUser( idx );
}

void UserManager::ProcessLogin( LoginInfo* info )
{
	if( nullptr == info )
	{
		Logger::Log( "�߸��� �α��� ��û �߻� - null" );
		return;
	}

	size_t reqUser = info->idx;
	if( ST_NOLOGIN != m_userList[ reqUser ].state )
	{
		Logger::Log( "�߸��� �α��� ��û �߻� - ������ NOLOGIN ���°� �ƴ�" );
		return;
	}

	//DB ���̵�, ��й�ȣ ����

	//�ߺ� �α��� üũ
	/*if (0 != userIDSet.count(info->id))
	{
		Logger::Log("�ߺ� �α��� �õ�");
		SC_PACKET_LOGIN_FAIL packet(LOGIN_FAIL_SAME_ID);
		MiniGameServer::Instance().SendPacket(&userList[reqUser], &packet);
		return;
	}*/

	//�α��� OK!!
	m_userList[ reqUser ].id = info->id;
	m_userList[ reqUser ].state = ST_IDLE;
	m_userList[ reqUser ].characterType = info->characterType;
	Logger::Log( "���� �α��� ����" );

	SC_PACKET_LOGIN_OK packet;
	MiniGameServer::Instance().SendPacket( &m_userList[ reqUser ], &packet );

	delete info;
}

void UserManager::ProcessEnqueue( size_t idx )
{
	//���� ����üũ �־����
	if( ST_IDLE == m_userList[ idx ].state )
	{
		m_matchQueue.Enqueue( idx );
		m_userList[ idx ].state = ST_QUEUE;
		SC_PACKET_CHANGE_QUEUE packet{ true };
		MiniGameServer::Instance().SendPacket( &m_userList[ idx ], &packet );

		if( true == m_matchQueue.CanMakeMake() )
		{
			std::vector<size_t> matchUsers;
			if( false == m_matchQueue.MatchMake( matchUsers ) ) return;

			//��Ŵ������� �뺸.
			std::vector<User*> matchUserPtrs;
			for( auto userIdx : matchUsers )
			{
				matchUserPtrs.emplace_back( &m_userList[ userIdx ] );
				m_userList[ userIdx ].state = ST_PLAY;
			}
			RoomManager::Instance().PushJob( RMGR_CREATE, new CreateRoomInfo( matchUserPtrs ) );
			Logger::Log( "�ο��� ����, �� �Ŵ����� �� ���� ��û" );
		}
	}
}

void UserManager::ProcessDequeue( size_t idx )
{
	//���� ����üũ �־����
	if( ST_QUEUE == m_userList[ idx ].state )
	{
		m_matchQueue.Dequeue( idx );
		SC_PACKET_CHANGE_QUEUE packet{ false };
		MiniGameServer::Instance().SendPacket( &m_userList[ idx ], &packet );
		Logger::Log( "��ġť ��� ����" );
	}
}

void UserManager::ProcessLeaveRoom( size_t idx )
{
	switch( m_userList[ idx ].state )
	{
		case ST_PLAY:
			//������ �ٽ� �κ��
			m_userList[ idx ].state = ST_IDLE;
			break;

		case ST_DISCONN:
			//���� ���� �� ������ ���� ����
			DisconnectUser( idx );
			break;
	}
}

void UserManager::DisconnectUser( size_t idx )
{
	if( ST_DISCONN == m_userList[ idx ].state )
	{
		closesocket( m_userList[ idx ].socket );
		m_userList[ idx ].socket = INVALID_SOCKET;
		char nameBuf[ INET_ADDRSTRLEN ]{ 0, };
		inet_ntop( AF_INET, &m_userList[ idx ].addr.sin_addr, nameBuf, INET_ADDRSTRLEN );
		Logger::Log( "���� ���� : " + std::string( nameBuf ) + ":" + std::to_string( m_userList[ idx ].addr.sin_port ) );
		m_indexPool.push( idx );
	}
}
