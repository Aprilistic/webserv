#include "Http.hpp"

// keep alive 추가

Http::Http() : mStatus(CLIENT_ERROR_NOT_FOUND), mRes(ParsingIncompleted) {}

Http::~Http() {}

int Http::getStatus() const { return mStatus; }

void Http::waitRequest(std::string &buf) {
  if (mRes == ParsingIncompleted) {
    makeOneRequest(buf);

    // event 추가
  }
}

void Http::makeOneRequest(std::string &buf) {
  RequestParser parser;
  RequestSyntax syntax;

  mRes = parser.parse(mReq, buf.c_str(), buf.c_str() + buf.size());

  std::cout << "Request Content : " << mReq.mContent.size() << std::endl;
  for (std::vector<char>::iterator it = mReq.mContent.begin();
       it != mReq.mContent.end(); ++it) {
    std::cout << *it;
  }
  std::cout << std::endl;

  // if (mRes == ParsingCompleted) {
  //   mStatus = syntax.checksyntax(mReq);
  // }
}
