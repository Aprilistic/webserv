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

#define DEFAULT_ERROR_PAGE_PATH "/Users/euiclee/webserv/www/error/error.html"

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
  std::vector<char> GetCGIbufferToVector();

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
  void ResetCGIbuffer(void);
  void SetCGIbuffer(std::string &CGIResponseMessage);
  Request &GetRequest(void);
  Response &GetResponse(void);


  void SetRequest(eStatusCode state, int port, std::vector<char> &RecvBuffer);
  void HandleRequestType(int port);
  void HandleCGIRequest(int port);
  void HandleHTTPRequest(int port);
private:
  bool checkRedirect(int port);
  bool checkClientMaxBodySize(int port);
  bool checkLimitExcept(int port);

private:
  std::string mBuffer;
  std::string mCGIbuffer;
  Request mRequest;
  Response mResponse;
  RequestParser mRequestParser;
  int mFd;
  static int mFileID;
};

#endif