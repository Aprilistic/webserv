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

#define DEFAULT_ERROR_PAGE_PATH "./www/error/default.html"

class IRequestHandler;

class Http {
public:
  //   Http();
  Http(int socket, int port, std::string &sendBuffer, bool &keepAlive,
       int &remainingRequest);
  ~Http();

  eStatusCode ReadFile(const std::string &path);
  eStatusCode WriteFile(std::string &path, std::string &data,
                        eStatusCode pathType, bool append = false);

  eStatusCode CheckPathType(const std::string &path);

  void ResetAll(void);
  void ResetBuffer(void);
  Request &GetRequest(void);
  Response &GetResponse(void);
  ResponseParser &GetResponseParser(void);

  void ErrorHandle(eStatusCode errorStatus);
  void SetRequest(eStatusCode state, std::vector<char> &RecvBuffer);
  void HandleRequestType(void);
  void HandleCGIRequest(void);
  void HandleHTTPRequest(void);
  int GetPort(void);
  int GetSocket(void);
  std::string &GetSendBuffer(void);
  eStatusCode PriorityHeaders(void);
  void SendResponse(eStatusCode state);

private:
  bool checkRedirect(void);
  bool checkClientMaxBodySize(void);
  bool checkLimitExcept(void);

private:
  std::string mBuffer;
  Request mRequest;
  Response mResponse;
  RequestParser mRequestParser;
  ResponseParser mResponseParser;

  int mPort;
  int mSocket;
  bool &mKeepAlive;
  int &mRemainingRequest;
  std::string &mSendBufferRef;
};

#endif