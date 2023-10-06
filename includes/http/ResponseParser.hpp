#ifndef RESPONSEPARSER_HPP
#define RESPONSEPARSER_HPP

#include "Enum.hpp"
#include "Response.hpp"

class ResponseParser {
public:
  ResponseParser();
  ~ResponseParser();

  eParseResult parse(Response &resp, const char *begin, const char *end);

private:
  // static bool checkIfConnection(const Response::HeaderItem &item);
  static bool checkIfConnection(const std::pair<const std::string, std::string> &item);
  eParseResult consume(Response &resp, const char *begin, const char *end);
  // Check if a byte is an HTTP character.
  inline bool isChar(int c);
  // Check if a byte is an HTTP control character.
  inline bool isControl(int c);
  // Check if a byte is defined as an HTTP special character.
  inline bool isSpecial(int c);
  // Check if a byte is a digit.
  inline bool isDigit(int c);

public:
  size_t mContentSize;
  std::string mChunkSizeStr;
  std::string mHeaderName;
  std::string mHeaderValue;
  size_t mChunkSize;
  bool mChunked;
  int mState;
};

#endif
