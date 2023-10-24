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
  int mVersionMajor;
  int mVersionMinor;
  // std::vector<HeaderItem> mHeaders;
  std::multimap<std::string, std::string> mHeaders;
  // std::vector<char> mContent;
  std::string mBody;
  bool mKeepAlive;

  unsigned int mStatusCode;
  std::string mStatus;

  std::string mFilename;
};

#endif
