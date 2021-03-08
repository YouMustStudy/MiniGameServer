#pragma once
#include <string>
#include <ctime>
#include <regex>

/**
*@brief 현재 시간을 반환한다.
*@return [0000-00-00 HH:MM:SS] 형식의 문자열을 반환한다.
*/
std::string GetTime();

/**
*@brief 문자열에서 포트를 추출한다.
*@param[in] arg 포트를 추출할 문자열.
*@param[out] port 문자열에서 변환된 포트 [-32,768-32,767]
*@return 추출 성공 시 true, 실패 시 false.
*/
bool PortParse(char* arg, short& port);