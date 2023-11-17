#include "RequestParser.hpp"

RequestParser::RequestParser()
    : mState(RequestMethodStart), mContentsize(0), mChunkSize(0),
      mChunked(false) {}

RequestParser::~RequestParser() {}

eStatusCode RequestParser::Parse(Request &req, const char *begin,
                                 const char *end) {
  return consume(req, begin, end);
}

bool RequestParser::checkIfConnection(
    const std::pair<const std::string, std::string> &item) {
  return (item.first == "Connection");
}

eStatusCode RequestParser::consume(Request &req, const char *begin,
                                   const char *end) {
  while (begin != end) {
    char input = *begin++;

    if (input == '\0') {
      return PARSING_INCOMPLETED;
    }

    switch (mState) {
    case RequestMethodStart:
      if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        mState = RequestMethod;
        req.PushBackMethod(input);
      }
      break;
    case RequestMethod:
      if (input == ' ') {
        mState = RequestUriStart;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        req.PushBackMethod(input);
      }
      break;
    case RequestUriStart:
      if (req.GetMethod() != "GET" && req.GetMethod() != "POST" &&
          req.GetMethod() != "DELETE" && req.GetMethod() != "HEAD") {
        return CLIENT_ERROR_METHOD_NOT_ALLOWED;
      }
      if (isControl(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        mState = RequestUri;
        req.PushBackUri(input);
      }
      break;
    case RequestUri:
      if (input == ' ') {
        mState = RequestHttpVersion_h;
      } else if (input == '\r') {
        return CLIENT_ERROR_BAD_REQUEST;
      } else if (isControl(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        req.PushBackUri(input);
      }
      break;
    case RequestHttpVersion_h:
      if (req.GetUri().empty()) {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      if (input == 'H') {
        mState = RequestHttpVersion_ht;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_ht:
      if (input == 'T') {
        mState = RequestHttpVersion_htt;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_htt:
      if (input == 'T') {
        mState = RequestHttpVersion_http;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_http:
      if (input == 'P') {
        mState = RequestHttpVersion_slash;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_slash:
      if (input == '/') {
        req.SetVersionMajor(0);
        req.SetVersionMinor(0);
        mState = RequestHttpVersion_majorStart;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_majorStart:
      if (isDigit(input)) {
        req.SetVersionMajor(input - '0');
        mState = RequestHttpVersion_major;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_major:
      if (input == '.') {
        mState = RequestHttpVersion_minorStart;
      } else if (isDigit(input)) {
        req.SetVersionMajor(req.GetVersionMajor() * 10 + input - '0');
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_minorStart:
      if (isDigit(input)) {
        req.SetVersionMinor(input - '0');
        mState = RequestHttpVersion_minor;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case RequestHttpVersion_minor:
      if (input == '\r') {
        mState = ResponseHttpVersion_newLine;
      } else if (isDigit(input)) {
        req.SetVersionMinor(req.GetVersionMinor() * 10 + input - '0');
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ResponseHttpVersion_newLine:
      if (req.GetVersionMajor() != 1 && req.GetVersionMinor() != 1) {
        return SERVER_ERROR_HTTP_VERSION_NOT_SUPPORTED;
      }
      if (input == '\n') {
        mState = HeaderLineStart;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case HeaderLineStart:
      if (input == '\r') {
        mState = ExpectingNewline_3;
      } else if (!req.GetHeaders().empty() && (input == ' ' || input == '\t')) {
        mState = HeaderLws;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        mHeaderName.clear();
        mHeaderName.push_back(input);
        mState = HeaderName;
      }
      break;
    case HeaderLws:
      if (input == '\r') {
        mState = ExpectingNewline_2;
      } else if (input == ' ' || input == '\t') {
      } else if (isControl(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        mState = HeaderValue;
        mHeaderValue.push_back(input);
      }
      break;
    case HeaderName:
      if (input == ':') {
        mState = SpaceBeforeHeaderValue;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        mHeaderName.push_back(input);
      }
      break;
    case SpaceBeforeHeaderValue:
      if (input == ' ') {
        mState = HeaderValue;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case HeaderValue:
      if (input == '\r') {
        if (req.GetMethod() == "POST") {
          if (strcasecmp(mHeaderName.c_str(), "Content-Length") == 0) {
            req.SetContentLength(atoi(mHeaderValue.c_str()));
            mContentsize = atoi(mHeaderValue.c_str());
            // req.mContent.reserve(mContentsize);
          } else if (strcasecmp(mHeaderName.c_str(), "Transfer-Encoding") ==
                     0) {
            if (strcasecmp(mHeaderValue.c_str(), "Chunked") == 0)
              mChunked = true;
          } else if (strcasecmp(mHeaderName.c_str(), "Content-Type") == 0) {
            req.SetContentType(mHeaderValue);
          }
        }
        std::string tmp = mHeaderName;
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        if (tmp == "host") {
          req.SetHost(mHeaderValue);
        }
        req.InsertHeader(mHeaderName, mHeaderValue);
        mHeaderValue.clear();
        mState = ExpectingNewline_2;
      } else if (isControl(input)) {
        return CLIENT_ERROR_BAD_REQUEST;
      } else {
        mHeaderValue.push_back(input);
      }
      break;
    case ExpectingNewline_2:
      if (input == '\n') {
        mState = HeaderLineStart;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ExpectingNewline_3: {
      std::multimap<std::string, std::string> headers = req.GetHeaders();
      std::multimap<std::string, std::string>::iterator it =
          std::find_if(headers.begin(), headers.end(), checkIfConnection);

      if (it != headers.end()) {
        if (strcasecmp(it->second.c_str(), "Keep-Alive") == 0) {
          req.SetKeepAlive(true);
        } else {
          req.SetKeepAlive(false);
        }
      } else if (req.GetVersionMajor() > 1 ||
                 (req.GetVersionMajor() == 1 && req.GetVersionMinor() == 1)) {
        req.SetKeepAlive(true);
      }

      if (mChunked) {
        req.SetChunked(true);
        mState = ChunkSize;
      } else if (mContentsize == 0) {
        if (input == '\n') {
          mRemainingBuffer.assign(begin, end);
          return PARSING_COMPLETED;
        } else
          return CLIENT_ERROR_BAD_REQUEST;
      } else {
        mState = Post;
      }
      break;
    }
    case Post:
      --mContentsize;
      req.PushBackContent(input);

      if (mContentsize == 0) {
        mRemainingBuffer.assign(begin, end);
        return PARSING_COMPLETED;
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
        return CLIENT_ERROR_BAD_REQUEST;
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
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkExtensionValue:
      if (isalnum(input) || input == ' ') {
        // skip
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkSizeNewLine:
      if (input == '\n') {
        mChunkSize = strtol(mChunkSizeStr.c_str(), NULL, 16);
        mChunkSizeStr.clear();
        // req.mContent.reserve(req.mContent.size() + mChunkSize);

        if (mChunkSize == 0) {
          mState = ChunkSizeNewLine_2;
        } else {
          mState = ChunkData;
        }
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkSizeNewLine_2:
      if (input == '\r') {
        mState = ChunkSizeNewLine_3;
      } else if (isalpha(input)) {
        mState = ChunkTrailerName;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkSizeNewLine_3:
      if (input == '\n') {
        mRemainingBuffer.assign(begin, end);
        return PARSING_COMPLETED;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkTrailerName:
      if (isalnum(input)) {
        // skip
      } else if (input == ':') {
        mState = ChunkTrailerValue;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkTrailerValue:
      if (isalnum(input) || input == ' ') {
        // skip
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkData:
      req.PushBackContent(input);

      if (--mChunkSize == 0) {
        mState = ChunkDataNewLine_1;
      }
      break;
    case ChunkDataNewLine_1:
      if (input == '\r') {
        mState = ChunkDataNewLine_2;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    case ChunkDataNewLine_2:
      if (input == '\n') {
        mState = ChunkSize;
      } else {
        return CLIENT_ERROR_BAD_REQUEST;
      }
      break;
    default:
      return CLIENT_ERROR_BAD_REQUEST;
    }
  }

  return PARSING_INCOMPLETED;
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

std::string const &RequestParser::GetRemainingBuffer() const {
  return mRemainingBuffer;
}