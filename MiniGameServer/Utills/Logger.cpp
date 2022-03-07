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
		//[시간]msg 기록
		logMsg = GetTime() + msg;
	}
	else
	{
		//msg만 기록
		logMsg = msg;
	}

	//로그를 파일에 저장
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

	//[시간][유저주소][Error] Msg 의 양식으로 생성.
	std::string errMsg = GetTime() + msg + "[Error] " + reinterpret_cast<char*>( lpMsgBuf );

	//로그를 에러파일에 저장.
	std::ofstream logFile( LogFileName[ LOG_WSA ], std::ios::app );
	logFile << errMsg << std::endl;
	LocalFree( lpMsgBuf );

	//콘솔 및 로그파일에 저장.
	Log( errMsg, false );
}

void Logger::SetLogPath( int type, const std::string& path )
{
	//해당 로그파일의 경로 설정
	if( type >= 0 &&
		type < LOG_COUNT );
	{
		LogFileName[ type ] = path;
	}
}
