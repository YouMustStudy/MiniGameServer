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
���� �Ŵ���
������ ����, ����, ��ġ����ŷ�� ó���ϴ� ��ü
�̱��� ��ü�̴�
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

	void ProcessAccept(AcceptInfo* info);		//���� ����
	void ProcessDisconnect(size_t idx);			//���� ����
	void ProcessLogin(LoginInfo* info);			//���� �α���
	void ProcessEnqueue(size_t idx);			//��ġ ���
	void ProcessDequeue(size_t idx);			//��ġ ����
	void ProcessLeaveRoom(size_t idx);			//�濡�� �����ٴ� �뺸�� �޾��� �� ȣ��
	void DisconnectUser(size_t idx);			//���� ���� ó��(ProcessDisconnect ���� ȣ��)

	std::array<User, MAX_USER_SIZE> userList;	//���� ��ü �迭
	std::stack<size_t> indexPool;				//�������� ����� �� �ִ� ���� Ǯ
	HANDLE workerIOCP{INVALID_HANDLE_VALUE};	//IOCP �ڵ�(���� ��Ͽ�)

	MatchQueue matchQueue;	//��ġ����ŷ ��⿭
};