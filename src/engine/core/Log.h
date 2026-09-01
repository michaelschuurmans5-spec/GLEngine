#pragma once 

#include <iostream>
#include <string>


enum class LogLevel {
	Info,
	Warning,
	Error
};

class Log {
public:
	static void Init();
	static void Print(
		LogLevel level,
		const std::string& sender,
		const std::string& message
	);
};

// CORE ENGINE LOGGIN MACROS
#define ENGINE_INFO(...)  Log::Print(LogLevel::Info, "CORE", __VA_ARGS__)
#define ENGINE_WARN(...)  Log::Print(LogLevel::Warning, "CORE",    __VA_ARGS__)
#define ENGINE_ERROR(...) Log::Print(LogLevel::Error,   "CORE",    __VA_ARGS__)

// SANDBOX GAME CLIENT LOGGING MACROS
#define CLIENT_INFO(...)    Log::Print(LogLevel::Info,    "CLIENT",  __VA_ARGS__)
#define CLIENT_WARN(...)    Log::Print(LogLevel::Warning, "CLIENT",  __VA_ARGS__)
#define CLIENT_ERROR(...)   Log::Print(LogLevel::Error,   "CLIENT",  __VA_ARGS__)