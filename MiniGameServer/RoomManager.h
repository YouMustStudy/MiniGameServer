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
*@brief ������ ���� �ʱ����� ����.
*/
struct CreateRoomInfo
{
	CreateRoomInfo( const std::vector<User*>& userList ) : users( userList ) {};
	std::vector<User*> users; ///< �������.
};

/**
*@brief ��Ŵ���. ��(��ġ)�� ����, ������ ó���Ѵ�.
*@author Gurnwoo Kim.
*/
class RoomManager : public LockFreeQueue
{
private:
	std::array<DMRoom, MAX_ROOM_SIZE> m_roomList{}; ///< �� ���.
	std::stack<size_t>                m_indexPool;  ///< �� �ĺ��ڿ� Ǯ.

	/**
	*@brief ������.
	*/
	RoomManager();

	/**
	*@brief �� ������ ó���Ѵ�.
	*@param[in] info �� ��������.
	*/
	void ProcessCreateRoom( CreateRoomInfo* info );

	/**
	*@brief �� �ı��� ó���Ѵ�.
	*@param[in] idx �� �ε���.
	*/
	void ProcessDestroyRoom( size_t idx );

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
	virtual ~RoomManager() {};

	/**
	*@brief �̱��� ��ü�� ��ȯ�Ѵ�.
	*@return �� �Ŵ���.
	*/
	static RoomManager& Instance();

	/**
	*@brief ���� ��ȯ�Ѵ�.
	*@param[in] idx �� �ε���.
	*@return ��.
	*/
	DMRoom* GetRoom( size_t idx );
};
