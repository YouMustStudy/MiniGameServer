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
*@brief 접속 정보
*/
struct AcceptInfo
{
	SOCKET      socket; ///< 소켓
	SOCKADDR_IN addr;   ///< 주소
};

/**
*@brief 로그인 정보
*/
struct LoginInfo
{
	LoginInfo(size_t idx, const char* id, CHARACTER_TYPE characterType) : idx(idx), id(id), characterType(characterType) {};
	size_t         idx;           ///< 인덱스
	std::string    id;            ///< 이름
	CHARACTER_TYPE characterType; ///< 캐릭터 종류
};

/**
*@brief 유저 매니저. 유저의 접속, 종료, 매치메이킹을 처리한다. 싱글턴.
*@author Gurnwoo Kim.
*/
class UserManager : public LockFreeQueue
{
private:
	std::array<User, MAX_USER_SIZE> m_userList;	                          ///< 유저 객체 배열
	std::stack<size_t>              m_indexPool;				          ///< 서버에서 사용할 수 있는 유저 풀
	HANDLE                          m_workerIOCP{ INVALID_HANDLE_VALUE }; ///< IOCP 핸들(소켓 등록용)
	MatchQueue                      m_matchQueue;	                      ///< 매치메이킹 대기열

	/**
	*@brief 생성자.
	*/
	UserManager();

	/**
	*@brief 유저 접속을 처리한다.
	*@param[in] info 접속 정보.
	*/
	void ProcessAccept( AcceptInfo* info );

	/**
	*@brief 유저 종료를 처리한다.
	*@param[in] idx 유저 인덱스.
	*/
	void ProcessDisconnect( size_t idx );

	/**
	*@brief 남은 유저 종료 절차를 처리한다.
	*@param[in] idx 유저 인덱스.
	*/
	void DisconnectUser( size_t idx );

	/**
	*@brief 유저 로그인를 처리한다.
	*@param[in] idx 유저 인덱스.
	*/
	void ProcessLogin( LoginInfo* info );

	/**
	*@brief 매칭 큐에 등록한다.
	*@param[in] idx 유저 인덱스.
	*/
	void ProcessEnqueue( size_t idx );

	/**
	*@brief 매칭 큐에서 나온다.
	*@param[in] idx 유저 인덱스.
	*/
	void ProcessDequeue( size_t idx );

	/**
	*@brief 방을 나가는 처리를 한다.
	*@param[in] idx 유저 인덱스.
	*/
	void ProcessLeaveRoom( size_t idx );

protected:
	/**
	*@brief 작업을 처리한다.
	*param[in] job 작업.
	*/
	virtual void ProcessJob( Job job ) override;

public:
	/**
	*@brief 소멸자.
	*/
	virtual ~UserManager() {};

	/**
	*@brief 싱글턴 객체를 반환한다.
	*@return 유저 매니저.
	*/
	static UserManager& Instance();

	/**
	*@brief IOCP 핸들을 설정한다.
	*@param[in] iocp 핸들.
	*/
	void SetIOCPHandle( HANDLE iocp ) { m_workerIOCP = iocp; };

	/**
	*@brief 유저를 획득한다.
	*@param[in] 유저 인덱스.
	*/
	User* GetUser( size_t idx ) { return &m_userList[ idx ]; };

	/**
	*@brief 매칭에 필요한 최소 유저 수를 설정한다.
	*@param[in] num 설정할 숫자.
	*/
	void SetRequireUserNum( size_t num );
};
