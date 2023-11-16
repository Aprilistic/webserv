#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>

class Http;

enum LogLevel { info, warn, error, debug };

enum LogType { request, response };

void Log(LogLevel level, LogType type, Http &http);

void Log(LogLevel level, std::string message);

#endif