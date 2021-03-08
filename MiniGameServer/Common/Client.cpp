#include "CLIENT.h"
#include "..\Utills\Logger.h"

int Client::BUFFER_SIZE = 1024;

void Client::SetRecv()
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