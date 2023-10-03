#ifndef HTTP_HPP
#define HTTP_HPP

#include "Core.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "RequestSyntax.hpp"

class Http {
public:
  Http();
  ~Http();
  int getStatus() const;
  void receiveRequest(std::string &buf);
  void httpProcess();

private:
  Request mReq;
  eParseResult mRes;
  eStatustCode mStatus;
  // std::string mRemainingBuffer;
};

#endif