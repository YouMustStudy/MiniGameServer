#pragma once
#include <unordered_map>
#include <list>
#include <mutex>
#include <vector>
#include "ServerConfig.h"


/**
매치메이킹 큐

리스트로 큐를 구현하였으며 
빠른 삭제를 위해 unordered_map에 iterator를 저장, 삭제한다.
*/

class MatchQueue
{
	using IdxMapper = std::unordered_map<size_t, std::list<size_t>::iterator>;
	using UserQueue = std::list<size_t>;

public:
	void Enqueue(size_t idx);
	void Dequeue(size_t idx);

	bool CanMakeMake();
	bool MatchMake(std::vector<size_t>& users);

private:
	size_t	m_matchNum{ REQUIRE_USER_NUM };		// 매치가 성립하는 인원 수
	IdxMapper m_idxMapper;		// 유저 인덱스 + 큐의 iterator 매핑 테이블
	UserQueue m_userQueue;		// 실제 유저 대기열
};