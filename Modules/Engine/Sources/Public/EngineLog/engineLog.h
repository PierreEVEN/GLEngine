#pragma once

#include <string>

#define GFullLog(verbosity, category, message) EngineLog::PrintLog(verbosity, std::string(category) + " (" + std::to_string(__LINE__) + ") ", message)
#define GLogDisplay(message) EngineLog::PrintLog(LogVerbosity::Display, typeid(*this).name(), message)
#define GLogWarning(message) EngineLog::PrintLog(LogVerbosity::Warning, typeid(*this).name(), message)
#define GLogError(message) EngineLog::PrintLog(LogVerbosity::Error, typeid(*this).name(), message)
#define GLogAssert(message) EngineLog::PrintLog(LogVerbosity::Assert, std::string(typeid(*this).name()) + " (" + std::to_string(__LINE__) + ")", message)

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