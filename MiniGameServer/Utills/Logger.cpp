#include "Logger.h"

std::string Logger::LogFileName[ LOG_COUNT ]
{
	"NormalLog.txt",
	"WSALog.txt"
};

void Logger::Log( const std::string& msg, bool time )
{
	std::string logMsg;
	if( true == time )
	{
		//[�ð�]msg ���
		logMsg = GetTime() + msg;
	}
	else
	{
		//msg�� ���
		logMsg = msg;
	}

	//�α׸� ���Ͽ� ����
	std::cout << logMsg << std::endl;
	std::ofstream logFile( LogFileName[ LOG_NORMAL ], std::ios::app );
	logFile << logMsg << std::endl;
}

void Logger::WsaLog( const std::string& msg, int errNo )
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errNo,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );

	//[�ð�][�����ּ�][Error] Msg �� ������� ����.
	std::string errMsg = GetTime() + msg + "[Error] " + reinterpret_cast<char*>( lpMsgBuf );

	//�α׸� �������Ͽ� ����.
	std::ofstream logFile( LogFileName[ LOG_WSA ], std::ios::app );
	logFile << errMsg << std::endl;
	LocalFree( lpMsgBuf );

	//�ܼ� �� �α����Ͽ� ����.
	Log( errMsg, false );
}

void Logger::SetLogPath( int type, const std::string& path )
{
	//�ش� �α������� ��� ����
	if( type >= 0 &&
		type < LOG_COUNT );
	{
		LogFileName[ type ] = path;
	}
}
