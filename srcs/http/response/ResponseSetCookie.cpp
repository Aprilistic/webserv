#include "Http.hpp"
#include "ResponseParser.hpp"
#include "String.hpp"
#include "WebServer.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

static int monthToNumber(const std::string &month) {
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  for (int i = 0; i < 12; ++i) {
    if (month == months[i]) {
      return i;
    }
  }
  return -1; // Invalid month
}

static bool parseDate(const std::string &dateString, struct tm &tm) {
  std::istringstream dateStream(dateString);
  std::string dayOfWeek, month, timezone;
  int day, year, hour, minute, second;

  if (!(dateStream >> dayOfWeek >> day >> month >> year >> hour >> minute >>
        second >> timezone)) {
    return false; // Failed to parse
  }

  tm.tm_year = year - 1900;
  tm.tm_mon = monthToNumber(month);
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;
  tm.tm_isdst = -1; // Not dealing with daylight saving time

  return true;
}

static std::string trim(const std::string &str) {
  size_t first = str.find_first_not_of(' ');
  if (first == std::string::npos)
    return "";

  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

static std::map<std::string, std::string>
parseCookie(const std::string &cookie) {
  std::map<std::string, std::string> cookieMap;
  std::istringstream tokenStream(cookie);
  std::string token;

  while (std::getline(tokenStream, token, ';')) {
    std::istringstream keyValuePairStream(token);
    std::string key, value;

    if (std::getline(keyValuePairStream, key, '=')) {
      if (std::getline(keyValuePairStream, value)) {
        // Trim key and value
        key = trim(key);
        value = trim(value);

        cookieMap[key] = value;
      }
    }
  }

  return cookieMap;
}

void ResponseParser::setCookie(Http &http) {
  static unsigned int number;

  std::multimap<std::string, std::string> headers =
      http.GetRequest().GetHeaders();
  std::multimap<std::string, std::string>::iterator it;

  // find cookie from db
  // if not exist or expired, create cookie
  // else update cookie

  bool isExist = false;
  std::string cookieFileName = "./cookie/";

  it = headers.find("Cookie");
  if (it != headers.end()) {
    std::map<std::string, std::string> cookieMap = parseCookie(it->second);
    if (cookieMap.count("id")) {
      if (access(cookieMap["id"].c_str(), F_OK | R_OK) == 0) {
        isExist = true;
        cookieFileName = cookieMap["id"];
      }
    }
  }

  if (isExist == false) { // create cookie
    std::string hostname = http.GetRequest().GetHost();
    cookieFileName += hostname + "_" + ToString(number++) + ".cookie";
    number %= 987654321;
  }

  std::ofstream ofs(cookieFileName.c_str(), std::ios::out | std::ios::trunc);
  if (ofs.is_open()) {
    std::multimap<std::string, std::string>::const_iterator it =
        http.GetResponse().GetHeaders().find("Date");
    std::string responseTime = it->second;
    ofs << responseTime;
    ofs.close();
  }

  std::string cookie =
      "id=" + cookieFileName + "; Max-Age=" + ToString(SESSION_TIMEOUT);
  http.GetResponse().InsertHeader("Set-Cookie", cookie);
}