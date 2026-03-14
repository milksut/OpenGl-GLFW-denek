#pragma	once

#include <string>
#include <iostream>
#include <ctime>
#include <fstream>
#include <cstdarg>

enum class LogLevel {
	INFO,
	WARNING,
	ERROR,
	DEBUG,
	FATAL
};

class Logger {
public:

	void log(LogLevel level, const char* file, int line, const char* format, ...) {
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);
		time_t now = time(0);
		char* dt = ctime(&now);
		std::string dtStr(dt);
		dtStr.pop_back();
		std::cout << getLogLevelColor(level) << dtStr << " [" << getLogLevelString(level) << "] " << "(" << file << ":" << line << ") " << buffer << "\033[0m" << std::endl;
		logFile << dtStr << " [" << getLogLevelString(level) << "] " << "(" << file << ":" << line << ") " << buffer << std::endl;
	}

	static Logger& getInstance() {
		static Logger instance;
		return instance;
	}

private:

	std::string getLogLevelString(LogLevel level) {
		switch (level) {
		case LogLevel::INFO: return "INFO";
		case LogLevel::WARNING: return "WARNING";
		case LogLevel::ERROR: return "ERROR";
		case LogLevel::DEBUG: return "DEBUG";
		case LogLevel::FATAL: return "FATAL";
		default: return "UNKNOWN";
		}
	}

	std::ofstream logFile;
	Logger() {
		time_t now = time(0);
		tm* ltm = localtime(&now);

		std::string filename = "log_" +
			std::to_string(1900 + ltm->tm_year) + "-" +
			std::to_string(1 + ltm->tm_mon) + "-" +
			std::to_string(ltm->tm_mday) + "_" +
			std::to_string(ltm->tm_hour) + "-" +
			std::to_string(ltm->tm_min) + "-" +
			std::to_string(ltm->tm_sec) + ".txt";

		logFile.open(filename);
		if (!logFile.is_open()) {
			std::cerr << "Failed to open log file!" << std::endl;
		}
	}

	std::string getLogLevelColor(LogLevel level) {
		switch (level) {
		case LogLevel::INFO: return "\033[32m"; // Green
		case LogLevel::WARNING: return "\033[33m"; // Yellow
		case LogLevel::ERROR: return "\033[31m"; // Red
		case LogLevel::DEBUG: return "\033[34m"; // Blue
		case LogLevel::FATAL: return "\033[35m"; // Magenta
		default: return "\033[0m"; // Reset
		}
	}

};

#define LOG_ERROR(message) Logger::getInstance().log(message, LogLevel::ERROR, __FILE__, __LINE__)
#define LOG_WARNING(message) Logger::getInstance().log(message, LogLevel::WARNING, __FILE__, __LINE__)
#define LOG_INFO(message) Logger::getInstance().log(message, LogLevel::INFO, __FILE__, __LINE__)
#define LOG_DEBUG(message) Logger::getInstance().log(message, LogLevel::DEBUG, __FILE__, __LINE__)
#define LOG_FATAL(message) Logger::getInstance().log(message, LogLevel::FATAL, __FILE__, __LINE__)
