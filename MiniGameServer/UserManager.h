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

/**
*@brief ���� ����
*/
struct AcceptInfo
{
	SOCKET      socket; ///< ����
	SOCKADDR_IN addr;   ///< �ּ�
};

/**
*@brief �α��� ����
*/
struct LoginInfo
{
	LoginInfo(size_t idx, const char* id, CHARACTER_TYPE characterType) : idx(idx), id(id), characterType(characterType) {};
	size_t         idx;           ///< �ε���
	std::string    id;            ///< �̸�
	CHARACTER_TYPE characterType; ///< ĳ���� ����
};

/**
*@brief ���� �Ŵ���. ������ ����, ����, ��ġ����ŷ�� ó���Ѵ�. �̱���.
*@author Gurnwoo Kim.
*/
class UserManager : public LockFreeQueue
{
private:
	std::array<User, MAX_USER_SIZE> m_userList;	                          ///< ���� ��ü �迭
	std::stack<size_t>              m_indexPool;				          ///< �������� ����� �� �ִ� ���� Ǯ
	HANDLE                          m_workerIOCP{ INVALID_HANDLE_VALUE }; ///< IOCP �ڵ�(���� ��Ͽ�)
	MatchQueue                      m_matchQueue;	                      ///< ��ġ����ŷ ��⿭

	/**
	*@brief ������.
	*/
	UserManager();

	/**
	*@brief ���� ������ ó���Ѵ�.
	*@param[in] info ���� ����.
	*/
	void ProcessAccept( AcceptInfo* info );

	/**
	*@brief ���� ���Ḧ ó���Ѵ�.
	*@param[in] idx ���� �ε���.
	*/
	void ProcessDisconnect( size_t idx );

	/**
	*@brief ���� ���� ���� ������ ó���Ѵ�.
	*@param[in] idx ���� �ε���.
	*/
	void DisconnectUser( size_t idx );

	/**
	*@brief ���� �α��θ� ó���Ѵ�.
	*@param[in] idx ���� �ε���.
	*/
	void ProcessLogin( LoginInfo* info );

	/**
	*@brief ��Ī ť�� ����Ѵ�.
	*@param[in] idx ���� �ε���.
	*/
	void ProcessEnqueue( size_t idx );

	/**
	*@brief ��Ī ť���� ���´�.
	*@param[in] idx ���� �ε���.
	*/
	void ProcessDequeue( size_t idx );

	/**
	*@brief ���� ������ ó���� �Ѵ�.
	*@param[in] idx ���� �ε���.
	*/
	void ProcessLeaveRoom( size_t idx );

protected:
	/**
	*@brief �۾��� ó���Ѵ�.
	*param[in] job �۾�.
	*/
	virtual void ProcessJob( Job job ) override;

public:
	/**
	*@brief �Ҹ���.
	*/
	virtual ~UserManager() {};

	/**
	*@brief �̱��� ��ü�� ��ȯ�Ѵ�.
	*@return ���� �Ŵ���.
	*/
	static UserManager& Instance();

	/**
	*@brief IOCP �ڵ��� �����Ѵ�.
	*@param[in] iocp �ڵ�.
	*/
	void SetIOCPHandle( HANDLE iocp ) { m_workerIOCP = iocp; };

	/**
	*@brief ������ ȹ���Ѵ�.
	*@param[in] ���� �ε���.
	*/
	User* GetUser( size_t idx ) { return &m_userList[ idx ]; };

	/**
	*@brief ��Ī�� �ʿ��� �ּ� ���� ���� �����Ѵ�.
	*@param[in] num ������ ����.
	*/
	void SetRequireUserNum( size_t num );
};
