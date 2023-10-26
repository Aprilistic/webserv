#include "ResponseParser.hpp"

ResponseParser::ResponseParser()
    : mState(ResponseStatusStart), mContentSize(0), mChunkSize(0),
      mChunked(false) {}

ResponseParser::~ResponseParser() {}

eStatusCode ResponseParser::Parse(Response &resp, const char *begin,
                                  const char *end) {
  return consume(resp, begin, end);
}

// bool ResponseParser::checkIfConnection(const Response::HeaderItem &item) {
//   return strcasecmp(item.name.c_str(), "Connection") == 0;
// }

bool ResponseParser::checkIfConnection(
    const std::pair<const std::string, std::string> &item) {
  return (item.first == "Connection");
}

eStatusCode ResponseParser::consume(Response &resp, const char *begin,
                                    const char *end) {
  while (begin != end) {
    char input = *begin++;

    switch (mState) {
    case ResponseStatusStart:
      if (input != 'H') {
        return PARSING_ERROR;
      } else {
        mState = ResponseHttpVersion_ht;
      }
      break;
    case ResponseHttpVersion_ht:
      if (input == 'T') {
        mState = ResponseHttpVersion_htt;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_htt:
      if (input == 'T') {
        mState = ResponseHttpVersion_http;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_http:
      if (input == 'P') {
        mState = ResponseHttpVersion_slash;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_slash:
      if (input == '/') {
        resp.mVersionMajor = 0;
        resp.mVersionMinor = 0;
        mState = ResponseHttpVersion_majorStart;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_majorStart:
      if (isDigit(input)) {
        resp.mVersionMajor = input - '0';
        mState = ResponseHttpVersion_major;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_major:
      if (input == '.') {
        mState = ResponseHttpVersion_minorStart;
      } else if (isDigit(input)) {
        resp.mVersionMajor = resp.mVersionMajor * 10 + input - '0';
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_minorStart:
      if (isDigit(input)) {
        resp.mVersionMinor = input - '0';
        mState = ResponseHttpVersion_minor;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_minor:
      if (input == ' ') {
        mState = ResponseHttpVersion_statusCodeStart;
        resp.mStatusCode = 0;
      } else if (isDigit(input)) {
        resp.mVersionMinor = resp.mVersionMinor * 10 + input - '0';
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_statusCodeStart:
      if (isDigit(input)) {
        resp.mStatusCode = input - '0';
        mState = ResponseHttpVersion_statusCode;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_statusCode:
      if (isDigit(input)) {
        resp.mStatusCode = resp.mStatusCode * 10 + input - '0';
      } else {
        if (resp.mStatusCode < 100 || resp.mStatusCode > 999) {
          return PARSING_ERROR;
        } else if (input == ' ') {
          mState = ResponseHttpVersion_statusTextStart;
        } else {
          return PARSING_ERROR;
        }
      }
      break;
    case ResponseHttpVersion_statusTextStart:
      if (isChar(input)) {
        resp.mStatus += input;
        mState = ResponseHttpVersion_statusText;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_statusText:
      if (input == '\r') {
        mState = ResponseHttpVersion_newLine;
      } else if (isChar(input)) {
        resp.mStatus += input;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ResponseHttpVersion_newLine:
      if (input == '\n') {
        mState = HeaderLineStart;
      } else {
        return PARSING_ERROR;
      }
      break;
    case HeaderLineStart:
      if (input == '\r') {
        mState = ExpectingNewline_3;
      } else if (!resp.mHeaders.empty() && (input == ' ' || input == '\t')) {
        mState = HeaderLws;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return PARSING_ERROR;
      } else {
        // resp.mHeaders.push_back(Response::HeaderItem());
        // resp.mHeaders.back().name.reserve(16);
        // resp.mHeaders.back().value.reserve(16);
        // resp.mHeaders.back().name.push_back(input);
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
        return PARSING_ERROR;
      } else {
        mState = HeaderValue;
        // resp.mHeaders.back().value.push_back(input);
        mHeaderValue.push_back(input);
      }
      break;
    case HeaderName:
      if (input == ':') {
        mState = SpaceBeforeHeaderValue;
      } else if (!isChar(input) || isControl(input) || isSpecial(input)) {
        return PARSING_ERROR;
      } else {
        // resp.mHeaders.back().name.push_back(input);
        mHeaderName.push_back(input);
      }
      break;
    case SpaceBeforeHeaderValue:
      if (input == ' ') {
        mState = HeaderValue;
      } else {
        return PARSING_ERROR;
      }
      break;
    case HeaderValue:
      if (input == '\r') {
        // Response::HeaderItem &h = resp.mHeaders.back();

        // if (strcasecmp(h.name.c_str(), "Content-Length") == 0) {
        //   mContentSize = atoi(h.value.c_str());
        //   resp.mContent.reserve(mContentSize);
        // } else if (strcasecmp(h.name.c_str(), "Transfer-Encoding") == 0) {
        //   if (strcasecmp(h.value.c_str(), "mChunked") == 0)
        //     mChunked = true;
        // }
        if (strcasecmp(mHeaderName.c_str(), "Content-Length") == 0) {
          mContentSize = atoi(mHeaderValue.c_str());
          // resp.mContent.reserve(mContentSize);
          resp.mBody.reserve(mContentSize);
        } else if (strcasecmp(mHeaderName.c_str(), "Transfer-Encoding") == 0) {
          if (strcasecmp(mHeaderValue.c_str(), "mChunked") == 0)
            mChunked = true;
        }
        resp.mHeaders.insert(
            std::pair<std::string, std::string>(mHeaderName, mHeaderValue));
        mHeaderValue.clear();
        mState = ExpectingNewline_2;
      } else if (isControl(input)) {
        return PARSING_ERROR;
      } else {
        // resp.mHeaders.back().value.push_back(input);
        mHeaderValue.push_back(input);
      }
      break;
    case ExpectingNewline_2:
      if (input == '\n') {
        mState = HeaderLineStart;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ExpectingNewline_3: {
      // std::vector<Response::HeaderItem>::iterator it = std::find_if(
      //     resp.mHeaders.begin(), resp.mHeaders.end(), checkIfConnection);
      std::multimap<std::string, std::string>::iterator it = std::find_if(
          resp.mHeaders.begin(), resp.mHeaders.end(), checkIfConnection);

      if (it != resp.mHeaders.end()) {
        // if (strcasecmp(it->value.c_str(), "Keep-Alive") == 0) {
        //   resp.mKeepAlive = true;
        // } else // == Close
        // {
        //   resp.mKeepAlive = false;
        // }
        if (strcasecmp(it->second.c_str(), "Keep-Alive") == 0) {
          resp.mKeepAlive = true;
        } else // == Close
        {
          resp.mKeepAlive = false;
        }
      } else {
        if (resp.mVersionMajor > 1 ||
            (resp.mVersionMajor == 1 && resp.mVersionMinor == 1))
          resp.mKeepAlive = true;
      }

      if (mChunked) {
        mState = ChunkSize;
      } else if (mContentSize == 0) {
        if (input == '\n')
          return PARSING_COMPLETED;
        else
          return PARSING_ERROR;
      }

      else {
        mState = Post;
      }
      break;
    }
    case Post:
      --mContentSize;
      // resp.mContent.push_back(input);
      resp.mBody += input;

      if (mContentSize == 0) {
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
        return PARSING_ERROR;
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
        return PARSING_ERROR;
      }
      break;
    case ChunkExtensionValue:
      if (isalnum(input) || input == ' ') {
        // skip
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ChunkSizeNewLine:
      if (input == '\n') {
        mChunkSize = strtol(mChunkSizeStr.c_str(), NULL, 16);
        mChunkSizeStr.clear();
        // resp.mContent.reserve(resp.mContent.size() + mChunkSize);
        resp.mBody.reserve(resp.mBody.size() + mChunkSize);

        if (mChunkSize == 0)
          mState = ChunkSizeNewLine_2;
        else
          mState = ChunkData;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ChunkSizeNewLine_2:
      if (input == '\r') {
        mState = ChunkSizeNewLine_3;
      } else if (isalpha(input)) {
        mState = ChunkTrailerName;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ChunkSizeNewLine_3:
      if (input == '\n') {
        return PARSING_COMPLETED;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ChunkTrailerName:
      if (isalnum(input)) {
        // skip
      } else if (input == ':') {
        mState = ChunkTrailerValue;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ChunkTrailerValue:
      if (isalnum(input) || input == ' ') {
        // skip
      } else if (input == '\r') {
        mState = ChunkSizeNewLine;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ChunkData:
      // resp.mContent.push_back(input);
      resp.mBody += input;

      if (--mChunkSize == 0) {
        mState = ChunkDataNewLine_1;
      }
      break;
    case ChunkDataNewLine_1:
      if (input == '\r') {
        mState = ChunkDataNewLine_2;
      } else {
        return PARSING_ERROR;
      }
      break;
    case ChunkDataNewLine_2:
      if (input == '\n') {
        mState = ChunkSize;
      } else {
        return PARSING_ERROR;
      }
      break;
    default:
      return PARSING_ERROR;
    }
  }

  return PARSING_INCOMPLETED;
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