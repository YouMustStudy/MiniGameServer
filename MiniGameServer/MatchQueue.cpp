#include "MatchQueue.h"
#include "Utills/Logger.h"

void MatchQueue::Enqueue( size_t idx )
{
	if( 0 == m_idxMapper.count( idx ) )
	{
		m_userQueue.emplace_back( idx );
		m_idxMapper.emplace( idx, std::prev( m_userQueue.end() ) );
		Logger::Log( "매치큐 등록" );
	}
}

void MatchQueue::Dequeue( size_t idx )
{
	if( 1 == m_idxMapper.count( idx ) )
	{
		m_userQueue.erase( m_idxMapper[ idx ] );
		m_idxMapper.erase( idx );
		Logger::Log( "매치큐 해제" );
	}
}

bool MatchQueue::CanMakeMake()
{
	return m_matchNum <= m_userQueue.size();
}

bool MatchQueue::MatchMake( std::vector<size_t>& users )
{
	if( true == CanMakeMake() )
	{
		users.clear();
		for( size_t i = 0; i < m_matchNum; ++i )
		{
			size_t idx = m_userQueue.front();
			m_userQueue.pop_front();
			m_idxMapper.erase( idx );
			users.emplace_back( idx );
		}
		return true;
	}
	return false;
}
