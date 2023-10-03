#include "Http.hpp"

// keep alive 추가

Http::Http() : mStatus(CLIENT_ERROR_NOT_FOUND), mRes(ParsingIncompleted) {}

Http::~Http() {}

int Http::getStatus() const { return mStatus; }

void Http::receiveRequest(std::string &buf) {
  RequestParser parser;
  RequestSyntax syntax;

  mRes = parser.parse(mReq, buf.c_str(), buf.c_str() + buf.size());
  // event

  if (mRes == ParsingCompleted) {
    std::cout << "ParsingCompleted" << std::endl;
    std::cout << parser.getRemainingBuffer() << std::endl;
    mStatus = syntax.checksyntax(mReq);
    // mRemainingBuffer =
  }
}

void Http::httpProcess() {
  // Request
  // RequestParser
  // RequestSyntax
  // for ()
  // Response
}