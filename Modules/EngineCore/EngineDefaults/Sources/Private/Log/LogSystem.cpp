#include <chrono>
#include <ctime>
#include <iostream>
#include <windows.h>


#include <sys/types.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <Log/LogSystem.h>

void EngineLog::PrintLog(LogVerbosity verbosity, std::string LogCategory, std::string message)
{
	std::time_t t = std::time(0);   // get time now
	std::tm now;
	localtime_s(&now, &t);
	std::string logDate = 
		"<" 
		+ std::to_string(now.tm_year + 1900) 
		+ '-' 
		+ std::to_string(now.tm_mon + 1) 
		+ '-' 
		+ std::to_string(now.tm_mday) 
		+ " " 
		+ std::to_string(now.tm_hour)
		+ '-'
		+ std::to_string(now.tm_min)
		+ '-'
		+ std::to_string(now.tm_sec)
		+ '>';

	std::string logType =
		verbosity == LogVerbosity::Display ? "Log"
		: verbosity == LogVerbosity::Warning ? "Warning"
		: verbosity == LogVerbosity::Error ? "Error"
		: verbosity == LogVerbosity::Assert ? "Assert" : "null";
	
	std::string logText = "[" + logType + " - " + LogCategory +"] " + logDate + " : " + message;

	int messageColor =
		verbosity == LogVerbosity::Display ? 7
		: verbosity == LogVerbosity::Warning ? 6
		: verbosity == LogVerbosity::Error ? 4
		: verbosity == LogVerbosity::Assert ? 5 : 9;

	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, messageColor);
	std::cout << logText << std::endl;


	if (verbosity == LogVerbosity::Assert)
	{
		SetConsoleTextAttribute(hConsole, 1);
#ifdef WIN32 
		if (IsDebuggerPresent())
		{
			__debugbreak(); exit(-1);
		}
#endif
		for (int i = 5; i > 0; --i)
		{
			std::cout << "closing in " << i << "..." << std::endl;
			Sleep(1000);
		}
		SetConsoleTextAttribute(hConsole, 7);
		exit(1);
	}
}

