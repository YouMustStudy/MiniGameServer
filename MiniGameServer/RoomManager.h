#pragma once
#include <array>
#include <stack>
#include <chrono>
#include "Common/User.h"
#include "Rooms/DMRoom.h"
#include "LockFreeQueue.h"


enum RoomManagerJobType
{
	RMGR_CREATE,
	RMGR_DESTROY
};

/**
*@brief 생성될 룸의 초기정보 정의.
*/
struct CreateRoomInfo
{
	CreateRoomInfo( const std::vector<User*>& userList ) : users( userList ) {};
	std::vector<User*> users; ///< 유저목록.
};

/**
*@brief 룸매니저. 방(매치)의 생성, 삭제를 처리한다.
*@author Gurnwoo Kim.
*/
class RoomManager : public LockFreeQueue
{
private:
	std::array<DMRoom, MAX_ROOM_SIZE> m_roomList{}; ///< 룸 목록.
	std::stack<size_t>                m_indexPool;  ///< 룸 식별자용 풀.

	/**
	*@brief 생성자.
	*/
	RoomManager();

	/**
	*@brief 룸 생성을 처리한다.
	*@param[in] info 룸 생성정보.
	*/
	void ProcessCreateRoom( CreateRoomInfo* info );

	/**
	*@brief 룸 파괴를 처리한다.
	*@param[in] idx 룸 인덱스.
	*/
	void ProcessDestroyRoom( size_t idx );

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
	virtual ~RoomManager() {};

	/**
	*@brief 싱글턴 객체를 반환한다.
	*@return 룸 매니저.
	*/
	static RoomManager& Instance();

	/**
	*@brief 룸을 반환한다.
	*@param[in] idx 룸 인덱스.
	*@return 룸.
	*/
	DMRoom* GetRoom( size_t idx );
};
