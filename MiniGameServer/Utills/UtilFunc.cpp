#include "UtilFunc.h"
std::string GetTime()
{
	//현재 로컬타임 반환
	time_t current_time = time(nullptr);
	struct tm current_tm;
	localtime_s(&current_tm, &current_time);

	int year = current_tm.tm_year + 1900;
	int month = current_tm.tm_mon + 1;
	int day = current_tm.tm_mday;
	int hour = current_tm.tm_hour;
	int minute = current_tm.tm_min;
	int second = current_tm.tm_sec;

	char buf[34];
	sprintf_s(buf, "[%4d-%02d-%02d %02d:%02d:%02d] ", year, month, day, hour, minute, second);
	return buf;
}

bool PortParse(char * arg, short & port)
{
	std::regex numCheck(R"([0-9]{1,5})");
	//short 범위에 맞춰 5자리까지만 입력받는다.
	if (true == std::regex_match(arg, numCheck))
	{
		int inputValue = std::stoi(arg);
		//int값이 ushort 범위 내에 있는 지 체크
		if (0 <= inputValue &&
			USHRT_MAX >= inputValue)
		{
			port = static_cast<short>(inputValue);
			return true;
		}
		else
		{
			//ushort 범위를 벗어나면 false.
			return false;
		}
	}
	//5자리 내 숫자가 아니면 false.
	return false;
}
