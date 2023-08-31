#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Core.hpp"

class Request {
public:
  Request();
  ~Request();

  struct HeaderItem {
    std::string name;
    std::string value;
  };

  std::string inspect() const;

public:
  std::string method;
  std::string uri;
  int versionMajor;
  int versionMinor;
  std::vector<HeaderItem> headers;
  std::vector<char> content;
  bool keepAlive;
};

#endif