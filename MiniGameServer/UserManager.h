#pragma once
#include <array>
#include <stack>
#include <unordered_set>
#include "Common/User.h"
#include "LockFreeQueue.h"
#include "MatchQueue.h"
#include "ServerConfig.h"
#include "protocol.h"

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
	LoginInfo(size_t idx, const char* id, CHARACTER_TYPE characterType) : idx(idx), id(id), characterType(characterType) {};
	size_t idx;
	std::string id;
	CHARACTER_TYPE characterType;
};

/**
유저 매니저
유저의 접속, 종료, 매치메이킹을 처리하는 객체
싱글턴 객체이다
*/
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

	void ProcessAccept(AcceptInfo* info);		//유저 접속
	void ProcessDisconnect(size_t idx);			//유저 종료
	void ProcessLogin(LoginInfo* info);			//유저 로그인
	void ProcessEnqueue(size_t idx);			//매치 등록
	void ProcessDequeue(size_t idx);			//매치 해제
	void ProcessLeaveRoom(size_t idx);			//방에서 나갔다는 통보를 받았을 시 호출
	void DisconnectUser(size_t idx);			//유저 종료 처리(ProcessDisconnect 이후 호출)

	std::array<User, MAX_USER_SIZE> userList;	//유저 객체 배열
	std::stack<size_t> indexPool;				//서버에서 사용할 수 있는 유저 풀
	HANDLE workerIOCP{INVALID_HANDLE_VALUE};	//IOCP 핸들(소켓 등록용)

	MatchQueue matchQueue;	//매치메이킹 대기열
};