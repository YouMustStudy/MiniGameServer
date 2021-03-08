#pragma once
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include "UtilFunc.h"

enum LOG_TYPE
{
	LOG_NORMAL, //�Ϲ����� ��Ȳ
	LOG_WSA,	//WSA���� ��Ȳ
	LOG_COUNT
};

class Logger
{
public:
	/**
	*@brief �ְܼ� ���Ͽ� [�ð�] msg �÷� ����ϴ� �α� �Լ�.
	*@param[in] msg ����� �޼���.
	*/
	static void Log(const std::string& msg, bool time = true);

	/**
	*@brief WSA���� ������ ���� �ֿܼ� ��� �� �α����Ϸ� �����Ѵ�. [�ð�][�����ּ�][Error] Msg �� ������� ����.
	*@param[in] msg ���� �ּ��� ���ڿ�.
	*@param[in] errNo ���� �ڵ�.
	*/
	static void WsaLog(const std::string& msg, int errNo);

	/**
	*@brief �α� ������ ����� ��� ���.
	*@param[in] type �н��� ������ �α��� Ÿ��, LOG_TYPE ����.
	*@param[in] path �α� ���� ���.
	*/
	static void SetLogPath(int type, const std::string& path);
private:
	static std::string LogFileName[LOG_COUNT];
};
