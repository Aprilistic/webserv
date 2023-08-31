#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Core.hpp"
#include "Request.hpp"

enum ParseResult { ParsingCompleted, ParsingIncompleted, ParsingError };
// The current state of the parser.
enum State {
  RequestMethodStart,
  RequestMethod,
  RequestUriStart,
  RequestUri,
  RequestHttpVersion_h,
  RequestHttpVersion_ht,
  RequestHttpVersion_htt,
  RequestHttpVersion_http,
  RequestHttpVersion_slash,
  RequestHttpVersion_majorStart,
  RequestHttpVersion_major,
  RequestHttpVersion_minorStart,
  RequestHttpVersion_minor,

  ResponseStatusStart,
  ResponseHttpVersion_ht,
  ResponseHttpVersion_htt,
  ResponseHttpVersion_http,
  ResponseHttpVersion_slash,
  ResponseHttpVersion_majorStart,
  ResponseHttpVersion_major,
  ResponseHttpVersion_minorStart,
  ResponseHttpVersion_minor,
  ResponseHttpVersion_spaceAfterVersion,
  ResponseHttpVersion_statusCodeStart,
  ResponseHttpVersion_spaceAfterStatusCode,
  ResponseHttpVersion_statusTextStart,
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

class RequestParser {
public:
  RequestParser();
  ~RequestParser();

  ParseResult parse(Request &req, const char *begin, const char *end);

private:
  static bool checkIfConnection(const Request::HeaderItem &item);
  ParseResult consume(Request &req, const char *begin, const char *end);
  // Check if a byte is an HTTP character.
  inline bool isChar(int c);
  // Check if a byte is an HTTP control character.
  inline bool isControl(int c);
  // Check if a byte is defined as an HTTP special character.
  inline bool isSpecial(int c);
  // Check if a byte is a digit.
  inline bool isDigit(int c);

public:
private:
  size_t contentSize;
  std::string chunkSizeStr;
  size_t chunkSize;
  bool chunked;
  int state;
};

#endif // LIBAHTTP_REQUESTPARSER_H