#pragma once
#include <unordered_map>
#include <list>
#include <mutex>
#include <vector>

class MatchQueue
{
	using IdxMapper = std::unordered_map<size_t, size_t>;
	using UserQueue = std::list<size_t>;

public:
	void Enqueue(size_t idx);
	void Dequeue(size_t idx);
	bool MatchMake(std::vector<size_t>& users);

private:
	size_t m_matchNum{4};
	IdxMapper m_idxMapper;
	UserQueue m_userQueue;
};