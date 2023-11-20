#include "Http.hpp"
#include "CGI.hpp"

Http::Http(int socket, int port, std::string &sendBuffer, bool &keepAlive,
           int &remainingRequest)
    : mSocket(socket), mPort(port), mSendBufferRef(sendBuffer),
      mKeepAlive(keepAlive), mRemainingRequest(remainingRequest) {}

Http::~Http() {}

void Http::RedirectURI() {
  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, mRequest.GetHost(), mRequest.GetUri());

  std::vector<std::string> redirectValues =
      location->FindValue(location, "return");
  if (redirectValues.empty()) {
    Log(warn, "Http: redirectValues is NULL");
    return (ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }

  eStatusCode state =
      static_cast<eStatusCode>(std::atoi(redirectValues[0].c_str()));

  mResponse.InsertHeader("Location", redirectValues[1]);

  SendResponse(state);
}

void Http::ErrorHandle(eStatusCode errorStatus) {
  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, mRequest.GetHost(), mRequest.GetUri());

  std::vector<std::string> configErrorPageValues =
      location->FindValue(location, "error_page");

  if (configErrorPageValues.size() > 1) {
    for (std::vector<std::string>::iterator it = configErrorPageValues.begin();
         it != configErrorPageValues.end() - 1; it++) {
      int errorCode;
      std::stringstream ss(*it);
      std::string errorPagePath;
      if (ss >> errorCode && errorCode == errorStatus) {
        errorPagePath = configErrorPageValues.back();
        ReadFile(errorPagePath);
        SendResponse(errorStatus);
        return;
      }
    }
  }
  // default error page respons
  ReadFile(DEFAULT_ERROR_PAGE_PATH);
  SendResponse(errorStatus);
}

eStatusCode Http::ReadFile(const std::string &path) {
  GetResponse().SetFilename(path);

  std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
  if (file.is_open()) {
    std::string tmp;
    tmp.assign((std::istreambuf_iterator<char>(file)),
               std::istreambuf_iterator<char>());
    GetResponse().SetBody(tmp);
    file.close();
    return (SUCCESSFUL_OK);
  } else {
    return (CLIENT_ERROR_NOT_FOUND);
  }
}

eStatusCode Http::WriteFile(std::string &path, std::string &data,
                            eStatusCode pathType) {
  if (pathType == PATH_IS_DIRECTORY) {
    // create random file name
    std::string fileName = "post_" + GenerateUniqueHash(path) + ".txt";

    path += fileName;
  }

  std::fstream file;
  if (pathType == PATH_IS_FILE) {
    file.open(path.c_str(), std::ios::app);
    if (file.is_open()) {
      file << data;
      file.close();
      return (SUCCESSFUL_OK);
    }
  } else {
    file.open(path.c_str(), std::ios::out);
    if (file.is_open()) {
      file << data;
      file.close();
      return (SUCCESSFUL_CREATERD);
    }
  }
  return (CLIENT_ERROR_NOT_FOUND);
}

eStatusCode Http::CheckPathType(const std::string &path) {
  struct stat info;

  if (stat(path.c_str(), &info) != 0) {
    if (errno == ENOENT) {
      return (PATH_NOT_FOUND);
    }
    return (PATH_INACCESSIBLE); // 권한 에러 Cannot access path
  } else if (info.st_mode & S_IFDIR) {
    return (PATH_IS_DIRECTORY); // 디렉토리 Directory
  } else if (info.st_mode & S_IFREG) {
    return (PATH_IS_FILE); // 파일 Regular file
  } else {
    return (PATH_UNKNOWN); // 디렉토리도 파일도 아닌 타입(소켓, 파이프, 심볼릭)
  }
}

bool Http::IsCgiRequest() {

  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, GetRequest().GetHost(), GetRequest().GetUri());
  if (location == NULL) {
    return (false);
  }

  std::vector<std::string> cgi_pass = location->FindValue(location, "cgi_pass");
  if (cgi_pass.size()) {
    return (true);
  }
  return (false);
}

void Http::SetRequest(eStatusCode state, std::vector<char> &RecvBuffer) {
  if (state != READ_OK) {
    Log(error, "Http: Read error");
    return (ErrorHandle(state));
  }

  std::string temp(RecvBuffer.begin(), RecvBuffer.end());
  mBuffer += temp;

  while (true) {
    eStatusCode ParseState = mRequestParser.Parse(
        mRequest, mBuffer.c_str(), mBuffer.c_str() + mBuffer.size());

    if (ParseState == PARSING_INCOMPLETED) {
      mBuffer.clear();
      return;
    } else if (ParseState == PARSING_COMPLETED) {
      // If Connection: close, never read again from socket

      if (mRequest.GetKeepAlive() == false) {
        mKeepAlive = false;
        struct kevent event;
        EV_SET(&event, mSocket, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
      }
      mRemainingRequest++; // remaining request count

      mBuffer = mRequestParser.GetRemainingBuffer();
      Log(info, request, *this);
      HandleRequestType();
    } else {
      mBuffer.clear();
      return (ErrorHandle(ParseState));
    }
  }
}

void Http::SendResponse(eStatusCode state) {
  mSendBufferRef += mResponseParser.MakeResponseMessage(*this, state);
  mRemainingRequest--;
  Log(info, response, *this);
  ResetAll();
}

void Http::HandleRequestType() {
  eStatusCode state = priorityHeaders();
  if (state == REDIRECT) {
    return RedirectURI(); // redirect 처리
  } else if (state != PRIORITY_HEADER_OK) {
    Log(warn, "Http: Priority header error");
    return ErrorHandle(state);
  }

  if (IsCgiRequest()) {
    handleCGIRequest();
  } else {
    handleHTTPRequest();
  }
}

void Http::handleCGIRequest() {
  SharedPtr<CGI> cgiRequest(new CGI(*this));

  mCGIList.push_back(cgiRequest);
  cgiRequest->CgiHandle();
}

void Http::handleHTTPRequest() {
  IRequestHandler *method = Router::Routing(*this);

  method->Handle(*this);
  delete (method);
}

eStatusCode Http::priorityHeaders() {
  if (checkRedirect()) {
    return REDIRECT;
  }
  if (checkClientMaxBodySize() == false) {
    return (CLIENT_ERROR_CONTENT_TOO_LARGE);
  }
  if (checkLimitExcept() == false) {
    return (CLIENT_ERROR_METHOD_NOT_ALLOWED);
  }
  return PRIORITY_HEADER_OK;
}

bool Http::checkRedirect() {
  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, mRequest.GetHost(), mRequest.GetUri());

  int redirectCode;
  std::string redirectPath;

  if (mRequest.GetUri() == "/") {
    std::vector<std::string> Topvalue;
    Topvalue =
        location->FindTopValue(location, "return", std::vector<std::string>());
    if (Topvalue.size()) {
      redirectCode = std::atoi(Topvalue[0].c_str());
      redirectPath = Topvalue[1];
    } else {
      return (false);
    }
  } else {
    std::vector<std::string> downValue;
    downValue = location->FindValue(location, "return");
    if (downValue.size()) {
      redirectCode = std::atoi(downValue[0].c_str());
      redirectPath = downValue[1];
    } else {
      return (false);
    }
  }
  return (true);
}

bool Http::checkClientMaxBodySize() {
  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, mRequest.GetHost(), mRequest.GetUri());

  std::vector<std::string> clientMaxBodySizeValues =
      location->FindValue(location, "client_max_body_size");
  if (clientMaxBodySizeValues.size() == 0) {
    return (true);
  }
  int valueSize =
      std::atoi(clientMaxBodySizeValues[0].c_str()); // overflow, k,M,G check

  if (mRequest.GetChunked() == true) {
    if (mRequest.GetContent().size() > valueSize) {
      return (false);
    }
  } else {
    if (clientMaxBodySizeValues.size()) {

      std::multimap<std::string, std::string> headers = mRequest.GetHeaders();
      std::multimap<std::string, std::string>::iterator it;
      it = headers.find("Content-Length");

      if (it != headers.end()) {
        int contentLength = std::atoi(it->second.c_str());
        if (contentLength > valueSize) {
          return (false);
        }
      }
    }
  }
  return (true);
}

bool Http::checkLimitExcept() {
  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, mRequest.GetHost(), mRequest.GetUri());

  std::vector<std::string> limitExceptValue =
      location->FindValue(location, "limit_except");
  if (limitExceptValue.size()) {
    if (std::find(limitExceptValue.begin(), limitExceptValue.end(),
                  mRequest.GetMethod()) == limitExceptValue.end()) {
      return (false);
    }
  } else if (mRequest.GetMethod() == "POST" || mRequest.GetMethod() == "HEAD") {
    return (false);
  }
  return (true);
}

void Http::ResetAll() {
  mRequest = Request();
  mResponse = Response();
  mRequestParser = RequestParser();
  mResponseParser = ResponseParser();
}

void Http::ResetBuffer() { mBuffer.clear(); }

Request &Http::GetRequest() { return mRequest; }

Response &Http::GetResponse() { return mResponse; }

ResponseParser &Http::GetResponseParser() { return mResponseParser; }

int Http::GetPort() { return (mPort); }

int Http::GetSocket() { return (mSocket); }

std::string &Http::GetSendBuffer() { return (mSendBufferRef); }