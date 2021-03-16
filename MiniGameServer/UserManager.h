#pragma once
#include <array>
#include <stack>
#include <unordered_set>
#include "Common/User.h"
#include "LockFreeQueue.h"
#include "MatchQueue.h"
#include "ServerConfig.h"

enum UserJobType
{
	USER_ACCEPT,
	USER_DISCONN,
	USER_LOGIN,
	USER_ENQUEUE,
	USER_DEQUEUE,
	USER_LEAVEROOM
};

struct AcceptInfo
{
	SOCKET socket;
	SOCKADDR_IN addr;
};

struct LoginInfo
{
	LoginInfo(size_t idx, const wchar_t* id, unsigned char characterType) : idx(idx), id(id), characterType(characterType) {};
	size_t idx;
	std::wstring id;
	unsigned char characterType;
};

class UserManager : public LockFreeQueue
{
public:
	virtual ~UserManager() {};

	static UserManager& Instance();
	void SetIOCPHandle(HANDLE iocp) { workerIOCP = iocp; };
	User* GetUser(size_t idx) { return &userList[idx]; };
protected:
	virtual void ProcessJob(Job job) override;

private:
	UserManager();

	void ProcessAccept(AcceptInfo* info);
	void ProcessDisconnect(size_t idx);
	void ProcessLogin(LoginInfo* info);
	void ProcessEnqueue(size_t idx);
	void ProcessDequeue(size_t idx);
	void ProcessLeaveRoom(size_t idx);

	void DisconnectUser(size_t idx);

	std::array<User, MAX_USER_SIZE> userList;
	std::unordered_set<std::wstring> userIDSet;
	std::stack<size_t> indexPool;
	HANDLE workerIOCP{INVALID_HANDLE_VALUE};

	MatchQueue matchQueue;
};