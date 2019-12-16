#ifndef ENGINELOG_H
#define ENGINELOG_H

#include <string>

#define GLog(verbosity, category, message) EngineLog::PrintLog(verbosity, category, message)

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
#endif