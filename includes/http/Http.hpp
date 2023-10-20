#ifndef HTTP_HPP
#define HTTP_HPP

#include "Common.hpp"
#include "ConfigMap.hpp"
#include "Core.hpp"
#include "Enum.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "RequestSyntax.hpp"
#include "Response.hpp"
#include "ResponseMessage.hpp"
#include "ResponseParser.hpp"

#include "IRequestHandler.hpp"
#include "Router.hpp"

class IRequestHandler;
class Http {
public:
  Http();
  ~Http();

  eStatusCode parseRequest(const std::vector<char> &buffer);
  //   std::vector<char> &parseResponse(Response response);

  eStatusCode requestParser(int &port, std::vector<char> &mRecvBuffer);
  eStatusCode priorityHeaders(int &port);
  eStatusCode setResponse(int &port);
  std::string getFileType();
  void MakeMandatoryHeaders();

  void ErrorHandle(int port, eStatusCode errorStatus);
  eStatusCode ReadFile(const std::string &path);
  eStatusCode WriteFile(const std::string &path);
  std::string AutoIndex(const std::string &path);

  bool CheckRedirect(int port);
  bool CheckClientMaxBodySize(int port);
  bool CheckLimitExcept(int port);

  eStatusCode CheckPathType(const std::string &path);

  void resetRequest(void);
  void resetResponse(void);
  void resetBuffer(void);
  std::string getBuffer(void);
  Request &getRequest(void);
  Response &getResponse(void);

private:
  std::string mBuffer;
  Request mRequest;
  Response mResponse;
  int mFd;
};

#endif