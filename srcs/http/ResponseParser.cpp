#include "ResponseParser.hpp"

ResponseParser::ResponseParser()
    : mState(ResponseStatusStart), mContentSize(0), mChunkSize(0),
      mChunked(false) {}

eResponseParseResult ResponseParser::parse(Response &resp, const char *begin,
                                           const char *end) {
  return consume(resp, begin, end);
}

bool ResponseParser::checkIfConnection(const Response::HeaderItem &item) {
  return strcasecmp(item.name.c_str(), "Connection") == 0;
}

eResponseParseResult ResponseParser::consume(Response &resp, const char *begin,
                                             const char *end) {
  while (begin != end) {
    char input = *begin++;

    switch (mState) {
    case ResponseStatusStart:
      if (input != 'H') {
        return ParsingError;
      } else {
        mState = ResponseHttpVersion_ht;
      }
      break;
    case ResponseHttpVersion_ht:
      if (input == 'T') {
        mState = ResponseHttpVersion_htt;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_htt:
      if (input == 'T') {
        mState = ResponseHttpVersion_http;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_http:
      if (input == 'P') {
        mState = ResponseHttpVersion_slash;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_slash:
      if (input == '/') {
        resp.versionMajor = 0;
        resp.versionMinor = 0;
        mState = ResponseHttpVersion_majorStart;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_majorStart:
      if (isDigit(input)) {
        resp.versionMajor = input - '0';
        mState = ResponseHttpVersion_major;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_major:
      if (input == '.') {
        mState = ResponseHttpVersion_minorStart;
      } else if (isDigit(input)) {
        resp.versionMajor = resp.versionMajor * 10 + input - '0';
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_minorStart:
      if (isDigit(input)) {
        resp.versionMinor = input - '0';
        mState = ResponseHttpVersion_minor;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_minor:
      if (input == ' ') {
        mState = ResponseHttpVersion_statusCodeStart;
        resp.statusCode = 0;
      } else if (isDigit(input)) {
        resp.versionMinor = resp.versionMinor * 10 + input - '0';
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_statusCodeStart:
      if (isDigit(input)) {
        resp.statusCode = input - '0';
        mState = ResponseHttpVersion_statusCode;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_statusCode:
      if (isDigit(input)) {
        resp.statusCode = resp.statusCode * 10 + input - '0';
      } else {
        if (resp.statusCode < 100 || resp.statusCode > 999) {
          return ParsingError;
        } else if (input == ' ') {
          mState = ResponseHttpVersion_statusTextStart;
        } else {
          return ParsingError;
        }
      }
      break;
    case ResponseHttpVersion_statusTextStart:
      if (isChar(input)) {
        resp.status += input;
        mState = ResponseHttpVersion_statusText;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_statusText:
      if (input == '\r') {
        mState = ResponseHttpVersion_newLine;
      } else if (isChar(input)) {
        resp.status += input;
      } else {
        return ParsingError;
      }
      break;
    case ResponseHttpVersion_newLine:
      if (input == '\n') {
        mState = HeaderLineStart;
      } else {
        return ParsingError;
      }
      break;
    case HeaderLineStart:
      if (input == '\r') {
        mState = ExpectingNewline_3;
      } else if (!resp.headers.empty() && (input == ' ' || input == '\t')) {
        mState = HeaderLws;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return ParsingError;
      } else {
        resp.headers.push_back(Response::HeaderItem());
        resp.headers.back().name.reserve(16);
        resp.headers.back().value.reserve(16);
        resp.headers.back().name.push_back(input);
        mState = HeaderName;
      }
      break;
    case HeaderLws:
      if (input == '\r') {
        mState = ExpectingNewline_2;
      } else if (input == ' ' || input == '\t') {
      } else if (isControl(input)) {
        return ParsingError;
      } else {
        mState = HeaderValue;
        resp.headers.back().value.push_back(input);
      }
      break;
    case HeaderName:
      if (input == ':') {
        mState = SpaceBeforeHeaderValue;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return ParsingError;
      } else {
        resp.headers.back().name.push_back(input);
      }
      break;
    case SpaceBeforeHeaderValue:
      if (input == ' ') {
        mState = HeaderValue;
      } else {
        return ParsingError;
      }
      break;
    case HeaderValue:
      if (input == '\r') {
        Response::HeaderItem &h = resp.headers.back();

        if (strcasecmp(h.name.c_str(), "Content-Length") == 0) {
          mContentSize = atoi(h.value.c_str());
          resp.content.reserve(mContentSize);
        } else if (strcasecmp(h.name.c_str(), "Transfer-Encoding") == 0) {
          if (strcasecmp(h.value.c_str(), "mChunked") == 0)
            mChunked = true;
        }
        mState = ExpectingNewline_2;
      } else if (isControl(input)) {
        return ParsingError;
      } else {
        resp.headers.back().value.push_back(input);
      }
      break;
    case ExpectingNewline_2:
      if (input == '\n') {
        mState = HeaderLineStart;
      } else {
        return ParsingError;
      }
      break;
    case ExpectingNewline_3: {
      std::vector<Response::HeaderItem>::iterator it = std::find_if(
          resp.headers.begin(), resp.headers.end(), checkIfConnection);

      if (it != resp.headers.end()) {
        if (strcasecmp(it->value.c_str(), "Keep-Alive") == 0) {
          resp.keepAlive = true;
        } else // == Close
        {
          resp.keepAlive = false;
        }
      } else {
        if (resp.versionMajor > 1 ||
            (resp.versionMajor == 1 && resp.versionMinor == 1))
          resp.keepAlive = true;
      }

      if (mChunked) {
        mState = ChunkSize;
      } else if (mContentSize == 0) {
        if (input == '\n')
          return ParsingCompleted;
        else
          return ParsingError;
      }

      else {
        mState = Post;
      }
      break;
    }
    case Post:
      --mContentSize;
      resp.content.push_back(input);

      if (mContentSize == 0) {
        return ParsingCompleted;
      }
      break;
    case ChunkSize:
      if (isalnum(input)) {
        mChunkSizeStr.push_back(input);
      } else if (input == ';') {
        mState = ChunkExtensionName;
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return ParsingError;
      }
      break;
    case ChunkExtensionName:
      if (isalnum(input) || input == ' ') {
        // skip
      } else if (input == '=') {
        mState = ChunkExtensionValue;
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return ParsingError;
      }
      break;
    case ChunkExtensionValue:
      if (isalnum(input) || input == ' ') {
        // skip
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return ParsingError;
      }
      break;
    case ChunkSizeNewLine:
      if (input == '\n') {
        mChunkSize = strtol(mChunkSizeStr.c_str(), NULL, 16);
        mChunkSizeStr.clear();
        resp.content.reserve(resp.content.size() + mChunkSize);

        if (mChunkSize == 0)
          mState = ChunkSizeNewLine_2;
        else
          mState = ChunkData;
      } else {
        return ParsingError;
      }
      break;
    case ChunkSizeNewLine_2:
      if (input == '\r') {
        mState = ChunkSizeNewLine_3;
      } else if (isalpha(input)) {
        mState = ChunkTrailerName;
      } else {
        return ParsingError;
      }
      break;
    case ChunkSizeNewLine_3:
      if (input == '\n') {
        return ParsingCompleted;
      } else {
        return ParsingError;
      }
      break;
    case ChunkTrailerName:
      if (isalnum(input)) {
        // skip
      } else if (input == ':') {
        mState = ChunkTrailerValue;
      } else {
        return ParsingError;
      }
      break;
    case ChunkTrailerValue:
      if (isalnum(input) || input == ' ') {
        // skip
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return ParsingError;
      }
      break;
    case ChunkData:
      resp.content.push_back(input);

      if (--mChunkSize == 0) {
        mState = ChunkDataNewLine_1;
      }
      break;
    case ChunkDataNewLine_1:
      if (input == '\r') {
        mState = ChunkDataNewLine_2;
      } else {
        return ParsingError;
      }
      break;
    case ChunkDataNewLine_2:
      if (input == '\n') {
        mState = ChunkSize;
      } else {
        return ParsingError;
      }
      break;
    default:
      return ParsingError;
    }
  }

  return ParsingIncompleted;
}

inline bool ResponseParser::isChar(int c) { return c >= 0 && c <= 127; }

inline bool ResponseParser::isControl(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

inline bool ResponseParser::isSpecial(int c) {
  switch (c) {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}

inline bool ResponseParser::isDigit(int c) { return c >= '0' && c <= '9'; }