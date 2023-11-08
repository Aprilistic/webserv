#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>

class Http;

enum LogLevel { info, warn, error, fatal };

enum LogType { request, response, etc };

void Log(LogLevel level, LogType type, std::string message, Http &http);

#endif