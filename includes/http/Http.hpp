#ifndef HTTP_HPP
#define HTTP_HPP

#include "Core.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "RequestParser.hpp"
#include "RequestSyntax.hpp"
#include "ResponseParser.hpp"
#include "ResponseMessage.hpp"

class Http {
public:
  Http();
  ~Http();
  int getStatus() const;
  void receiveRequest(std::string &buf);
  void httpProcess(std::string &buf);

private:
  Request mReq;
  eParseResult mRes;
  eStatustCode mStatus;
  std::string mTemp;
};

#endif