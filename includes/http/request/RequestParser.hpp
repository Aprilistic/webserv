#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Enum.hpp"
#include "Request.hpp"
class RequestParser {
public:
  RequestParser();
  ~RequestParser();

  eStatusCode Parse(Request &req, const char *begin, const char *end);
  std::string const &GetRemainingBuffer(void) const;

private:
  static bool
  checkIfConnection(const std::pair<const std::string, std::string> &item);
  eStatusCode consume(Request &req, const char *begin, const char *end);
  inline bool isChar(int c);    // Check HTTP character.
  inline bool isControl(int c); // Check HTTP control character.
  inline bool isSpecial(int c); // Check HTTP special character.
  inline bool isDigit(int c);   // Check a digit.

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