#pragma once
#include <string>
#include <ctime>
#include <regex>

/**
*@brief ���� �ð��� ��ȯ�Ѵ�.
*@return [0000-00-00 HH:MM:SS] ������ ���ڿ��� ��ȯ�Ѵ�.
*/
std::string GetTime();

/**
*@brief ���ڿ����� ��Ʈ�� �����Ѵ�.
*@param[in] arg ��Ʈ�� ������ ���ڿ�.
*@param[out] port ���ڿ����� ��ȯ�� ��Ʈ [-32,768-32,767]
*@return ���� ���� �� true, ���� �� false.
*/
bool PortParse(char* arg, short& port);