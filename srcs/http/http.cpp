#include "Http.hpp"

// keep alive 추가

Http::Http() : mStatus(CLIENT_ERROR_NOT_FOUND), mRes(ParsingIncompleted) {}

Http::~Http() {}

int Http::getStatus() const { return mStatus; }

void Http::receiveRequest(std::string &buf) {
  RequestParser parser;

  if (mRes == ParsingIncompleted && mTemp.size() > 0) {
    buf = mTemp + buf;
    mTemp.clear();
  }

  mRes = parser.parse(mReq, buf.c_str(), buf.c_str() + buf.size());
  // event

  if (mRes == ParsingIncompleted) {
    mTemp = parser.getRemainingBuffer();
  }
}

void Http::httpProcess(std::string &buf) {
  RequestSyntax syntax;

  // while (true) {
  //   switch (mRes) {
  //   case ParsingIncompleted: {
      // receiveRequest(buf);
    //   break;
    // }
    // case ParsingCompleted: {
      // mStatus = syntax.checksyntax(mReq);
      // mRes = ParsingIncompleted;
      Response res;
      ResponseParser parser;

      eParseResult resParse = parser.parse(res, buf.c_str(), buf.c_str() + buf.size());

      if (resParse == ParsingCompleted) {
        ResponseMessage resMsg;
        resMsg.MakeResponseMessage(res);
        std::cout << resMsg.getMessage();
      }
  //     break;
  //   }
  //   default:
  //     break;
  //   }
  // }
}