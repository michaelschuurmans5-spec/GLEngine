#include "core/Log.h"


void Log::Init() {
	// INIT
	Log::Print(LogLevel::Info, "SYSTEM", "Logger system initialized.");
}
// INTERPRETS REQUEST THEN DEBUG 
void Log::Print(LogLevel level, const std::string& sender,
	const std::string& message) {

	std::string prefix;

	switch (level) {
	    case LogLevel::Info: prefix = "[INFO]"; break;
		case LogLevel::Warning: prefix = "[WARN]"; break;
		case LogLevel::Error: prefix = "[ERROR]"; break;
	}

	// PRINTS STRING INFO CORE MESSAGE CONTENTS
	std::cout << prefix << "[" << sender << "]" << message << std::endl;

}