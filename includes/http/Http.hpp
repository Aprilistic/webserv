#ifndef HTTP_HPP
#define HTTP_HPP

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
class CGI;

class Http {
public:
  Http(int socket, int port, std::string &sendBuffer, bool &keepAlive,
       int &remainingRequest);
  ~Http();

  eStatusCode ReadFile(const std::string &path);
  eStatusCode WriteFile(std::string &path, std::string &data,
                        eStatusCode pathType);
  eStatusCode CheckPathType(const std::string &path);

  bool IsCgiRequest();
  void RedirectURI();
  void ErrorHandle(eStatusCode errorStatus);
  void HandleRequestType(void);
  void SetRequest(eStatusCode state, std::vector<char> &RecvBuffer);
  void SendResponse(eStatusCode state);

  void ResetAll(void);
  void ResetBuffer(void);

  int GetPort(void);
  int GetSocket(void);
  Request &GetRequest(void);
  Response &GetResponse(void);
  std::string &GetSendBuffer(void);
  ResponseParser &GetResponseParser(void);

private:
  void handleHTTPRequest(void);
  void handleCGIRequest(void);

  eStatusCode priorityHeaders(void);

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
  std::vector<SharedPtr<CGI> > mCGIList;
};

#endif