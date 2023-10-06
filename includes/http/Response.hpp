#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Core.hpp"

class Response {
public:
  Response();
  ~Response();

  // struct HeaderItem {
  //   std::string name;
  //   std::string value;
  // };

  std::string inspect() const;

public:
  int versionMajor;
  int versionMinor;
  // std::vector<HeaderItem> headers;
  std::multimap<std::string, std::string> headers;
  std::vector<char> content;
  bool keepAlive;

  unsigned int statusCode;
  std::string status;
};

#endif
