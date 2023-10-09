#include "HttpParser.hpp"

HttpParser::HttpParser() {}

HttpParser::~HttpParser() {}

eParseResult HttpParser::parseRequest(const std::vector<char> &buffer) {

  std::string tmp(buffer.begin(), buffer.end());
  mBuffer += tmp;

  RequestParser parser;
  eParseResult result =
      parser.parse(mRequest, mBuffer.c_str(), mBuffer.c_str() + mBuffer.size());

  if (result == ParsingIncompleted) {
    return ParsingIncompleted;
  } else if (result == ParsingCompleted) {
    mBuffer = parser.getRemainingBuffer();
    return ParsingCompleted;
  } else {
    return ParsingError;
  }
}

void HttpParser::resetRequest() { mRequest = Request(); }

void HttpParser::resetResponse() { mResponse = Response(); }

Request &HttpParser::getRequest() { return mRequest; }
