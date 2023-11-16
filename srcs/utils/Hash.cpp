#include "Hash.hpp"
#include "String.hpp"

unsigned long simpleHash(const std::string &str) {
  unsigned long hash = 5381;

  for (size_t i = 0; i < str.length(); i++) {
    hash = ((hash << 5) + hash) + static_cast<unsigned char>(str[i]);
  }
  return hash;
}

std::string GenerateUniqueHash(const std::string &uriPath) {
  // change current time to string
  std::time_t currentTime = std::time(0);
  std::ostringstream oss;
  oss << currentTime;
  std::string timeStr = oss.str();

  // concatenate uriPath and timeStr
  std::string concatStr = uriPath + timeStr;

  // hash the concatenated string
  return ToString(simpleHash(concatStr));
}