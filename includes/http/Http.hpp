#ifndef HTTP_HPP
#define HTTP_HPP

#include "Common.hpp"
#include "ConfigMap.hpp"
#include "Core.hpp"
#include "Enum.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
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

  // requestParser와 이름 겹침
  eStatusCode setOneRequest(int &port, std::vector<char> &mRecvBuffer);
  eStatusCode PriorityHeaders(int &port);
  eStatusCode SetResponse(int &port);
  std::string GetStatusMessage(eStatusCode errorStatus);
  std::string GetFileType();

  void MakeMandatoryHeaders();

  void ErrorHandle(int port, eStatusCode errorStatus);
  eStatusCode ReadFile(const std::string &path);
  eStatusCode WriteFile(std::string &path, std::string &data,
                        eStatusCode pathType, bool append = false);
  std::string AutoIndex(const std::string &path);

  eStatusCode CheckPathType(const std::string &path);

  void ResetRequest(void);
  void ResetResponse(void);
  void ResetBuffer(void);
  void ResetRequestParser(void);
  Request &GetRequest(void);
  Response &GetResponse(void);

private:
  bool checkRedirect(int port);
  bool checkClientMaxBodySize(int port);
  bool checkLimitExcept(int port);

private:
  std::string mBuffer;
  Request mRequest;
  Response mResponse;
  RequestParser mRequestParser;
  int mFd;
  static int mFileID;
};

#endif