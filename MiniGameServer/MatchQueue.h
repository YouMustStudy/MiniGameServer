#pragma once
#include <unordered_map>
#include <list>
#include <mutex>
#include <vector>
#include "ServerConfig.h"


/**
*@brief ��ġ����ŷ ť. ����Ʈ�� ť�� �����Ͽ����� ���� ������ ���� unordered_map�� iterator�� ����, �����Ѵ�.
*@author Gurnwoo Kim.
*/
class MatchQueue
{
private:
	using IdxMapper = std::unordered_map<size_t, std::list<size_t>::iterator>; ///< �ε��� ���� ������.
	using UserQueue = std::list<size_t>;                                       ///< ���� ť ������.

	size_t	  m_matchNum{ REQUIRE_USER_NUM }; ///< ��ġ�� �����ϴ� �ο� ��
	IdxMapper m_idxMapper;		              ///< ���� �ε��� + ť�� iterator ���� ���̺�
	UserQueue m_userQueue;		              ///< ���� ���� ��⿭

public:
	/**
	*@brief ��ġ�� �ʿ��� �ο����� �����Ѵ�.
	*@param[in] num �ο� ��.
	*/
	void SetRequireUserNum( size_t num ) { m_matchNum = num; };

	/**
	*@brief ��ġť�� ����Ѵ�.
	*@param[in] idx ���� �ε���.
	*/
	void Enqueue( size_t idx );

	/**
	*@brief ��ġť���� �����Ѵ�.
	*@param[in] idx ���� �ε���.
	*/
	void Dequeue( size_t idx );

	/**
	*@brief ��ġ ���� ���θ� ��ȯ�Ѵ�.
	*@return ��ġ ����ŷ ���� ����.
	*/
	bool CanMakeMake();

	/**
	*@brief ��ġ����ŷ�� �����Ѵ�.
	*@param[out] users ��ġ�� ������.
	*@return ���� ����.
	*/
	bool MatchMake( std::vector<size_t>& users );
};