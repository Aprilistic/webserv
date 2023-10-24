#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Enum.hpp"
#include "Request.hpp"
class RequestParser {
public:
  RequestParser();
  ~RequestParser();

  eStatusCode Parse(Request &req, const char *begin, const char *end);
  std::string GetRemainingBuffer(void);

private:
  // static bool checkIfConnection(const Request::HeaderItem &item);
  static bool
  checkIfConnection(const std::pair<const std::string, std::string> &item);
  eStatusCode consume(Request &req, const char *begin, const char *end);
  // Check if a byte is an HTTP character.
  inline bool isChar(int c);
  // Check if a byte is an HTTP control character.
  inline bool isControl(int c);
  // Check if a byte is defined as an HTTP special character.
  inline bool isSpecial(int c);
  // Check if a byte is a digit.
  inline bool isDigit(int c);

public:
  size_t mContentsize;

private:
  std::string mRemainingBuffer;
  std::string mChunkSizeStr;
  std::string mHeaderName;
  std::string mHeaderValue;
  size_t mChunkSize;
  bool mChunked;
  int mState;
};

#endif