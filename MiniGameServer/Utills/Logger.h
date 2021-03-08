#pragma once
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include "UtilFunc.h"

enum LOG_TYPE
{
	LOG_NORMAL, //일반적인 상황
	LOG_WSA,	//WSA오류 상황
	LOG_COUNT
};

class Logger
{
public:
	/**
	*@brief 콘솔과 파일에 [시간] msg 꼴로 출력하는 로그 함수.
	*@param[in] msg 출력할 메세지.
	*/
	static void Log(const std::string& msg, bool time = true);

	/**
	*@brief WSA관련 오류를 서버 콘솔에 출력 및 로그파일로 저장한다. [시간][유저주소][Error] Msg 의 양식으로 생성.
	*@param[in] msg 유저 주소의 문자열.
	*@param[in] errNo 오류 코드.
	*/
	static void WsaLog(const std::string& msg, int errNo);

	/**
	*@brief 로그 파일이 저장될 경로 등록.
	*@param[in] type 패스를 저장할 로그의 타입, LOG_TYPE 참조.
	*@param[in] path 로그 파일 경로.
	*/
	static void SetLogPath(int type, const std::string& path);
private:
	static std::string LogFileName[LOG_COUNT];
};
