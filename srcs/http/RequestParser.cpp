#include "RequestParser.hpp"

RequestParser::RequestParser()
    : mState(RequestMethodStart), mContentsize(0), mChunkSize(0),
      mChunked(false) {}

RequestParser::~RequestParser() {}

eParseResult RequestParser::parse(Request &req, const char *begin,
                                  const char *end) {
  return consume(req, begin, end);
}

bool RequestParser::checkIfConnection(const Request::HeaderItem &item) {
  return strcasecmp(item.name.c_str(), "Connection") == 0;
}

eParseResult RequestParser::consume(Request &req, const char *begin,
                                    const char *end) {
  while (begin != end) {
    char input = *begin++;

    if (input == '\0') {
      return ParsingIncompleted;
    }

    switch (mState) {
    case RequestMethodStart:
      if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return ParsingError;
      } else {
        mState = RequestMethod;
        req.mMethod.push_back(input);
      }
      break;
    case RequestMethod:
      if (input == ' ') {
        mState = RequestUriStart;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return ParsingError;
      } else {
        req.mMethod.push_back(input);
      }
      break;
    case RequestUriStart:
      if (isControl(input)) {
        return ParsingError;
      } else {
        mState = RequestUri;
        req.mUri.push_back(input);
      }
      break;
    case RequestUri:
      if (input == ' ') {
        mState = RequestHttpVersion_h;
      } else if (input == '\r') {
        req.mVersionMajor = 0;
        req.mVersionMinor = 9;

        return ParsingCompleted;
      } else if (isControl(input)) {
        return ParsingError;
      } else {
        req.mUri.push_back(input);
      }
      break;
    case RequestHttpVersion_h:
      if (input == 'H') {
        mState = RequestHttpVersion_ht;
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_ht:
      if (input == 'T') {
        mState = RequestHttpVersion_htt;
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_htt:
      if (input == 'T') {
        mState = RequestHttpVersion_http;
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_http:
      if (input == 'P') {
        mState = RequestHttpVersion_slash;
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_slash:
      if (input == '/') {
        req.mVersionMajor = 0;
        req.mVersionMinor = 0;
        mState = RequestHttpVersion_majorStart;
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_majorStart:
      if (isDigit(input)) {
        req.mVersionMajor = input - '0';
        mState = RequestHttpVersion_major;
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_major:
      if (input == '.') {
        mState = RequestHttpVersion_minorStart;
      } else if (isDigit(input)) {
        req.mVersionMajor = req.mVersionMajor * 10 + input - '0';
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_minorStart:
      if (isDigit(input)) {
        req.mVersionMinor = input - '0';
        mState = RequestHttpVersion_minor;
      } else {
        return ParsingError;
      }
      break;
    case RequestHttpVersion_minor:
      if (input == '\r') {
        mState = ResponseHttpVersion_newLine;
      } else if (isDigit(input)) {
        req.mVersionMinor = req.mVersionMinor * 10 + input - '0';
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
      } else if (!req.mHeaders.empty() && (input == ' ' || input == '\t')) {
        mState = HeaderLws;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return ParsingError;
      } else {
        req.mHeaders.push_back(Request::HeaderItem());
        req.mHeaders.back().name.reserve(16);
        req.mHeaders.back().value.reserve(16);
        req.mHeaders.back().name.push_back(input);
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
        req.mHeaders.back().value.push_back(input);
      }
      break;
    case HeaderName:
      if (input == ':') {
        mState = SpaceBeforeHeaderValue;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return ParsingError;
      } else {
        req.mHeaders.back().name.push_back(input);
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
        if (req.mMethod == "POST" || req.mMethod == "PUT") {
          Request::HeaderItem &h = req.mHeaders.back();

          if (strcasecmp(h.name.c_str(), "Content-Length") == 0) {
            mContentsize = atoi(h.value.c_str());
            req.mContent.reserve(mContentsize);
          } else if (strcasecmp(h.name.c_str(), "Transfer-Encoding") == 0) {
            if (strcasecmp(h.value.c_str(), "Chunked") == 0)
              mChunked = true;
          }
        }
        mState = ExpectingNewline_2;
      } else if (isControl(input)) {
        return ParsingError;
      } else {
        req.mHeaders.back().value.push_back(input);
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
      std::vector<Request::HeaderItem>::iterator it = std::find_if(
          req.mHeaders.begin(), req.mHeaders.end(), checkIfConnection);

      if (it != req.mHeaders.end()) {
        if (strcasecmp(it->value.c_str(), "Keep-Alive") == 0) {
          req.mKeepAlive = true;
        } else // == Close
        {
          req.mKeepAlive = false;
        }
      } else {
        if (req.mVersionMajor > 1 ||
            (req.mVersionMajor == 1 && req.mVersionMinor == 1))
          req.mKeepAlive = true;
      }

      if (mChunked) {
        mState = mChunkSize;
      } else if (mContentsize == 0) {
        if (input == '\n')
          return ParsingCompleted;
        else
          return ParsingError;
      } else {
        mState = Post;
      }
      break;
    }
    case Post:
      --mContentsize;
      req.mContent.push_back(input);

      if (mContentsize == 0) {
        mRemainingBuffer.assign(begin, end);
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
        req.mContent.reserve(req.mContent.size() + mChunkSize);

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
      req.mContent.push_back(input);

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
        mState = mChunkSize;
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

inline bool RequestParser::isChar(int c) { return c >= 0 && c <= 127; }

// Check if a byte is an HTTP control character.
inline bool RequestParser::isControl(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

// Check if a byte is defined as an HTTP special character.
inline bool RequestParser::isSpecial(int c) {
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

// Check if a byte is a digit.
inline bool RequestParser::isDigit(int c) { return c >= '0' && c <= '9'; }

std::string RequestParser::getRemainingBuffer() { return mRemainingBuffer; }