#ifndef RESPONSEPARSER_HPP
#define RESPONSEPARSER_HPP

#include "Response.hpp"

enum eParseResult { ParsingCompleted, ParsingIncompleted, ParsingError };

enum eState {
  ResponseStatusStart,
  ResponseHttpVersion_ht,
  ResponseHttpVersion_htt,
  ResponseHttpVersion_http,
  ResponseHttpVersion_slash,
  ResponseHttpVersion_majorStart,
  ResponseHttpVersion_major,
  ResponseHttpVersion_minorStart,
  ResponseHttpVersion_minor,
  ResponseHttpVersion_statusCodeStart,
  ResponseHttpVersion_statusCode,
  ResponseHttpVersion_statusTextStart,
  ResponseHttpVersion_statusText,
  ResponseHttpVersion_newLine,

  HeaderLineStart,
  HeaderLws,
  HeaderName,
  SpaceBeforeHeaderValue,
  HeaderValue,
  ExpectingNewline_2,
  ExpectingNewline_3,

  Post,
  ChunkSize,
  ChunkExtensionName,
  ChunkExtensionValue,
  ChunkSizeNewLine,
  ChunkSizeNewLine_2,
  ChunkSizeNewLine_3,
  ChunkTrailerName,
  ChunkTrailerValue,

  ChunkDataNewLine_1,
  ChunkDataNewLine_2,
  ChunkData,
};

class ResponseParser {
public:
  ResponseParser();
  ~ResponseParser();

  eParseResult parse(Response &resp, const char *begin, const char *end);

private:
  static bool checkIfConnection(const Response::HeaderItem &item);
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
  size_t mChunkSize;
  bool mChunked;
  int mState;
};

#endif
