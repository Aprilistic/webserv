#ifndef HTTP_HPP
#define HTTP_HPP

#include "Common.hpp"
#include "ConfigMap.hpp"
#include "Core.hpp"
#include "Enum.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ResponseParser.hpp"

#include "IRequestHandler.hpp"
#include "Router.hpp"
#include "CGI.hpp"

#define DEFAULT_ERROR_PAGE_PATH "/Users/euiclee/webserv/www/error/error.html"

class IRequestHandler;
class Http {
public:
  Http();
  ~Http();

  eStatusCode PriorityHeaders(int &port);

  void ErrorHandle(int port, eStatusCode errorStatus);
  eStatusCode ReadFile(const std::string &path);
  eStatusCode WriteFile(std::string &path, std::string &data,
                        eStatusCode pathType, bool append = false);

  eStatusCode CheckPathType(const std::string &path);

  void ResetAll(void);
  void ResetBuffer(void);
  Request &GetRequest(void);
  Response &GetResponse(void);


  void SetRequest(eStatusCode state, int port, std::vector<char> &RecvBuffer);
  void HandleRequestType(int port);
  void HandleCGIRequest(int port);
  void HandleHTTPRequest(int port);
private:
  void ResetRequest(void);
  void ResetResponse(void);
  void ResetRequestParser(void);
  bool checkRedirect(int port);
  bool checkClientMaxBodySize(int port);
  bool checkLimitExcept(int port);

private:
  std::string mBuffer;
  Request mRequest;
  Response mResponse;
  RequestParser mRequestParser;
  ResponseParser mResponseParser;
  int mFd;
  static int mFileID;
};

#endif