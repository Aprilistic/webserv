#ifndef RESPONSESYNTAX_HPP
#define RESPONSESYNTAX_HPP

#include "Core.hpp"

struct ResponseSyntax {
public:
  ResponseSyntax();
  ~ResponseSyntax();

  struct HeaderItem {
    std::string name;
    std::string value;
  };

  std::string inspect() const;

public:
  int versionMajor;
  int versionMinor;
  std::vector<HeaderItem> headers;
  std::vector<char> content;
  bool keepAlive;

  unsigned int statusCode;
  std::string status;
};

#endif
