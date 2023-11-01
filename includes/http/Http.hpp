#ifndef HTTP_HPP
#define HTTP_HPP

#include "CGI.hpp"
#include "Common.hpp"
#include "ConfigMap.hpp"
#include "Core.hpp"
#include "Enum.hpp"
#include "IRequestHandler.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ResponseParser.hpp"
#include "Router.hpp"

#define DEFAULT_ERROR_PAGE_PATH "/Users/euiclee/webserv/www/error/error.html"

class IRequestHandler;

class Http {
public:
  Http();
  ~Http();

  eStatusCode PriorityHeaders(int &port);

  void ErrorHandle(int port, eStatusCode errorStatus, int socket);
  eStatusCode ReadFile(const std::string &path);
  eStatusCode WriteFile(std::string &path, std::string &data,
                        eStatusCode pathType, bool append = false);

  eStatusCode CheckPathType(const std::string &path);

  void ResetAll(void);
  void ResetBuffer(void);
  Request &GetRequest(void);
  Response &GetResponse(void);
  ResponseParser &GetResponseParser(void);

  void SetRequest(eStatusCode state, int port, int socket,
                  std::vector<char> &RecvBuffer);
  void HandleRequestType(int port, int socket);
  void HandleCGIRequest(int port, int socket);
  void HandleHTTPRequest(int port, int socket);

  void SendResponse(eStatusCode state, int port, int socket);

private:
  bool checkRedirect(int port);
  bool checkClientMaxBodySize(int port);
  bool checkLimitExcept(int port);

private:
  std::string mBuffer;
  Request mRequest;
  Response mResponse;
  RequestParser mRequestParser;
  ResponseParser mResponseParser;

  static int mFileID;
};

#endif