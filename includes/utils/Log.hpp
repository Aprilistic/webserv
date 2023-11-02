#ifndef LOG_HPP
#define LOG_HPP

#include "Core.hpp"

class Log {
public:
	static void info(const char* message);
	static void warn(const char* message);
	static void error(const char* message);
	static void fatal(const char* message);
};

#endif