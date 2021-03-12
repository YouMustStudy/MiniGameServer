#pragma once
#include <unordered_map>
#include <list>
#include <mutex>
#include <vector>


/**
��ġ����ŷ ť

����Ʈ�� ť�� �����Ͽ����� 
���� ������ ���� unordered_map�� iterator�� ����, �����Ѵ�.
*/

class MatchQueue
{
	static constexpr size_t REQUIRE_USER_NUM = 3;
	using IdxMapper = std::unordered_map<size_t, std::list<size_t>::iterator>;
	using UserQueue = std::list<size_t>;

public:
	void Enqueue(size_t idx);
	void Dequeue(size_t idx);

	bool CanMakeMake();
	bool MatchMake(std::vector<size_t>& users);

private:
	size_t	m_matchNum{ REQUIRE_USER_NUM };		// ��ġ�� �����ϴ� �ο� ��
	IdxMapper m_idxMapper;		// ���� �ε��� + ť�� iterator ���� ���̺�
	UserQueue m_userQueue;		// ���� ���� ��⿭
};