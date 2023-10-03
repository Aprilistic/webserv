#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Core.hpp"
#include "Request.hpp"

class Response {
public:
  Response(Request &req, int status);
  ~Response();
  void makeResponse();
  void sendResponse();
  std::string getResponse() const;

private:
  void makeResponseVersion();
  void makeResponseStatusCode();
  void makeResponseServer();
  void makeResponseDate();
  void makeResponseContentLength();
  void makeResponseContentType();
  void makeResponseLastModified();
  void makeResponseConnection();
  void makeResponseBody();

private:
  Request &mReq;
  int mStatus;
  std::string mResponse;
};

#endif