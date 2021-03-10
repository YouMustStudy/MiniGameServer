#include "User.h"
#include "..\Utills\Logger.h"
#include "..\Rooms\DMRoom.h"

void User::SetRecv()
{
	recvOver.Reset();
	DWORD flag = 0;
	if (SOCKET_ERROR == WSARecv(socket, recvOver.Buffer(), 1, nullptr, &flag, recvOver.Overlapped(), nullptr))
	{
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			Logger::WsaLog("Send Error", errno);
	}
}