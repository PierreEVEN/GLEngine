#pragma once

#include <string>

#define GFullLog(verbosity, category, message) EngineLog::PrintLog(verbosity, category, message)
#define GDebugLog(message) EngineLog::PrintLog(LogVerbosity::Display, "Debug", message)

enum LogVerbosity
{
	Display,
	Warning,
	Error,
	Assert
};

class EngineLog
{
public:
	static void PrintLog(LogVerbosity verbosity, std::string LogCategory, std::string message);
};