#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Core.hpp"

class Request {
public:
  Request();
  ~Request();

  // struct HeaderItem {
  //   std::string name;
  //   std::string value;
  // };

  std::string inspect() const;

public:
  std::string mMethod;
  std::string mUri;
  int mVersionMajor;
  int mVersionMinor;
  // std::vector<HeaderItem> mHeaders;
  std::multimap<std::string, std::string> mHeaders;
  std::vector<char> mContent;

  std::string mHost;
  int mContentLength;
  bool mKeepAlive;
};

#endif