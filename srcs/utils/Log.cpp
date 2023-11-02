#include "Log.hpp"
#include "Http.hpp"

std::string requestLog(const Request &request) {
  std::string log;

  log += "[" + request.GetMethod() + " ";
  log += request.GetUri() + "]";

  return log;
}

std::string responseLog(const Response &response) {
  std::string log;

  log += "[" + std::to_string(response.GetStatusCode()) + " ";
  log += response.GetStatus() + " ";
  log += response.GetBody() + "]";

  return log;
}

void Log(LogLevel level, LogType type, std::string message, Http &http) {
  // time
  time_t now = time(0);
  struct tm *timeinfo = localtime(&now);
  char buf[80];
  strftime(buf, sizeof(buf), "%d/%b/%Y %H:%M:%S", timeinfo);

  // level
  std::string levelStr;

  switch (level) {
  case info:
    levelStr = "info";
    break;
  case warn:
    levelStr += YELLOW;
    levelStr += "warn";
    levelStr += RESET;
    break;
  case error:
    levelStr += RED;
    levelStr += "error";
    levelStr += RESET;
    break;
  case fatal:
    levelStr = "fatal";
    break;
  }

  std::cout << buf << " [" << levelStr << "] ";

  switch (type) {
  case etc:
    std::cout << RED << message << RESET << std::endl;
    break;
  case request:
    std::cout << "request " << CYAN << requestLog(http.GetRequest()) << RESET << std::endl;
    break;
  case response:
    std::cout << "response " << GREEN << responseLog(http.GetResponse()) << RESET
              << std::endl;
    break;
  }
}
