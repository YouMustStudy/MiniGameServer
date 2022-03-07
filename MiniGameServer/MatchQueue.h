#pragma once
#include <unordered_map>
#include <list>
#include <mutex>
#include <vector>
#include "ServerConfig.h"


/**
*@brief 매치메이킹 큐. 리스트로 큐를 구현하였으며 빠른 삭제를 위해 unordered_map에 iterator를 저장, 삭제한다.
*@author Gurnwoo Kim.
*/
class MatchQueue
{
private:
	using IdxMapper = std::unordered_map<size_t, std::list<size_t>::iterator>; ///< 인덱스 매퍼 재정의.
	using UserQueue = std::list<size_t>;                                       ///< 유저 큐 재정의.

	size_t	  m_matchNum{ REQUIRE_USER_NUM }; ///< 매치가 성립하는 인원 수
	IdxMapper m_idxMapper;		              ///< 유저 인덱스 + 큐의 iterator 매핑 테이블
	UserQueue m_userQueue;		              ///< 실제 유저 대기열

public:
	/**
	*@brief 매치에 필요한 인원수를 설정한다.
	*@param[in] num 인원 수.
	*/
	void SetRequireUserNum( size_t num ) { m_matchNum = num; };

	/**
	*@brief 매치큐에 등록한다.
	*@param[in] idx 유저 인덱스.
	*/
	void Enqueue( size_t idx );

	/**
	*@brief 매치큐에서 해제한다.
	*@param[in] idx 유저 인덱스.
	*/
	void Dequeue( size_t idx );

	/**
	*@brief 매치 가능 여부를 반환한다.
	*@return 매치 메이킹 가능 여부.
	*/
	bool CanMakeMake();

	/**
	*@brief 매치메이킹을 수행한다.
	*@param[out] users 매치된 유저들.
	*@return 성공 여부.
	*/
	bool MatchMake( std::vector<size_t>& users );
};