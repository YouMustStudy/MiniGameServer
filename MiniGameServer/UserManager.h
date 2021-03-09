#pragma once
#include <array>
#include <stack>
#include <unordered_set>
#include <WS2tcpip.h>
#include "Common/Client.h"
#include "LockFreeQueue.h"


enum UserJobType
{
	USER_ACCEPT,
	USER_DISCONN,
	USER_LOGIN,
	USER_ENTER_QUEUE,
	USER_EXIT_QUEUE
};

struct AcceptInfo
{
	SOCKET socket;
	SOCKADDR_IN addr;
};

struct LoginInfo
{
	size_t idx;
	wchar_t id[20];
	wchar_t pwd[20];
};

class UserManager : public LockFreeQueue
{
public:
	virtual ~UserManager() {};

	static UserManager& Instance();
	void SetIOCPHandle(HANDLE iocp) { workerIOCP = iocp; };
protected:
	virtual void ProcessJob(Job job) override;

private:
	UserManager();

	void ProcessAccept(AcceptInfo* info);
	void ProcessDisconnect(size_t idx);
	void ProcessLogin(LoginInfo* info);

	static constexpr size_t MAX_USER_SIZE = 1000;
	std::array<Client, MAX_USER_SIZE> userList{};
	std::unordered_set<std::wstring> userIDSet{};
	size_t curUserCnt{0};
	std::stack<size_t> indexPool;
	HANDLE workerIOCP{INVALID_HANDLE_VALUE};
};