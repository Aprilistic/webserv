#ifndef HTTPREQUESTPARSER_HPP
#define HTTPREQUESTPARSER_HPP

#include "Core.hpp"
#include "Request.hpp"

class HttpRequestParser {
public:
  HttpRequestParser();

  enum ParseResult { ParsingCompleted, ParsingIncompleted, ParsingError };

  ParseResult parse(Request &req, const char *begin, const char *end);

private:
  static bool checkIfConnection(const Request::HeaderItem &item);
  ParseResult consume(Request &req, const char *begin, const char *end);
  inline bool isChar(int c);
  inline bool isControl(int c);
  inline bool isSpecial(int c);
  inline bool isDigit(int c);

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
  } state;

  size_t contentSize;
  std::string chunkSizeStr;
  size_t chunkSize;
  bool chunked;
};

#endif
