#include "Log.hpp"
#include "Http.hpp"

std::string requestLog(const Request &request) {
  std::string log;

  log += "[" + request.GetMethod() + " ";
  log += request.GetUri();
  if (!request.GetContent().empty()) {
    log += " | ";
    log += ToString(request.GetContent().size()) + " bytes";
  }
  log += "]";
  return log;
}

std::string responseLog(const Response &response) {
  std::string log;

  log += "[" + ToString(response.GetStatusCode()) + " ";
  log += response.GetStatus() + " | ";
  if (response.GetBody().size() > 20) {
    if (response.GetBody().find("\n") && response.GetBody().find("\n") < 20) {
      log +=
          response.GetBody().substr(0, response.GetBody().find("\n")) + "...]";
    } else {
      log += response.GetBody().substr(0, 20) + "...]";
    }
  } else {
    log += response.GetBody() + "]";
  }

  return log;
}

void timeWithLevel(LogLevel level) {
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
  case debug:
    levelStr += PURPLE;
    levelStr += "debug";
    levelStr += RESET;
    break;
  }

  std::cout << buf << " [" << levelStr << "] ";
}

void Log(LogLevel level, LogType type, Http &http) {
  timeWithLevel(level);

  switch (type) {
  case request: {
    std::cout << "Request:  " << CYAN << requestLog(http.GetRequest()) << RESET
              << std::endl;
    break;
  }
  case response: {
    std::cout << "Response: ";
    if (http.GetResponse().GetStatusCode() >= 400) {
      std::cout << RED;
    } else if (http.GetResponse().GetStatusCode() >= 300) {
      std::cout << PURPLE;
    } else if (http.GetResponse().GetStatusCode() >= 200) {
      std::cout << GREEN;
    }
    std::cout << responseLog(http.GetResponse()) << RESET << std::endl;
  } break;
  }
}

void Log(LogLevel level, std::string message) {
  timeWithLevel(level);

  switch (level) {
  case info: {
    std::cout << message << std::endl;
    break;
  }
  case warn: {
    std::cout << YELLOW << message << RESET << std::endl;
    break;
  }
  case error: {
    std::cout << RED << message << RESET << std::endl;
    break;
  }
  case debug: {
    std::cout << PURPLE << message << RESET << std::endl;
    break;
  }
  }
}