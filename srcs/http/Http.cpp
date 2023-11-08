#include "Http.hpp"
#include "CGI.hpp"

int Http::mFileID = 0;
// Http::Http() {}

Http::Http(int socket, int port, std::string &sendBuffer, bool &keepAlive,
           int &remainingRequest)
    : mSocket(socket), mPort(port), mSendBufferRef(sendBuffer),
      mKeepAlive(keepAlive), mRemainingRequest(remainingRequest) {}

Http::~Http() {}

eStatusCode Http::PriorityHeaders() {
  if (checkRedirect()) {
    Log(error, etc, "REDIRECT", *this);
    return REDIRECT; // redirect path 로  response
  }
  if (checkClientMaxBodySize() == false) {
    return (CLIENT_ERROR_CONTENT_TOO_LARGE);
  }
  if (checkLimitExcept() == false) {
    return (CLIENT_ERROR_METHOD_NOT_ALLOWED);
  }
  return PRIORITY_HEADER_OK;
}

void Http::ErrorHandle(eStatusCode errorStatus) {

  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, mRequest.mHost, mRequest.mUri, mRequest.mMethod);

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
        // errorPagePath response
        return;
      }
    }
  }
  // default error page respons
  ReadFile(DEFAULT_ERROR_PAGE_PATH);
  SendResponse(errorStatus);
}

eStatusCode Http::ReadFile(const std::string &path) {
  GetResponse().mFilename = path;

  std::fstream file;

  file.open(path.c_str(), std::ios::in);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      GetResponse().mBody += line;
    }
    file.close();
    return (SUCCESSFUL_OK);
  } else {
    return (CLIENT_ERROR_NOT_FOUND);
  }
}

eStatusCode Http::WriteFile(std::string &path, std::string &data,
                            eStatusCode pathType, bool append) {
  if (pathType == PATH_IS_DIRECTORY) {
    // create random file name
    std::string fileName = "XXX";

    mkstemp(&fileName[0]);
    std::stringstream ss;
    ss << path << "/" << fileName;

    path = ss.str();
  }

  if (append) {
    std::fstream file;
    file.open(path.c_str(), std::ios::app);
    if (file.is_open()) {
      file << data;
      file.close();
      return (SUCCESSFUL_OK);
    } else {
      return (CLIENT_ERROR_NOT_FOUND);
    }
  } else {
    std::fstream file;
    file.open(path.c_str(), std::ios::out);
    if (file.is_open()) {
      file << data;
      file.close();
      return (SUCCESSFUL_CREATERD);
    } else {
      return (CLIENT_ERROR_NOT_FOUND);
    }
  }
}

bool Http::checkRedirect() {
  Node *location = Common::mConfigMap->GetConfigNode(
      mPort, mRequest.mHost, mRequest.mUri, mRequest.mMethod);

  int redirectCode;
  std::string redirectPath;

  if (mRequest.mUri == "/") {
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
      mPort, mRequest.mHost, mRequest.mUri, mRequest.mMethod);

  std::vector<std::string> clientMaxBodySizeValues =
      location->FindValue(location, "client_max_body_size");
  int valueSize =
      std::atoi(clientMaxBodySizeValues[0].c_str()); // overflow, k,M,G check

  if (mRequest.mChunked == true) {
    if (mRequest.mContent.size() > valueSize) {
      return (false);
    }
  } else {
    if (clientMaxBodySizeValues.size()) {

      std::multimap<std::string, std::string>::iterator it;
      it = mRequest.mHeaders.find("Content-Length");

      if (it != mRequest.mHeaders.end()) {
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
      mPort, mRequest.mHost, mRequest.mUri, mRequest.mMethod);

  std::vector<std::string> limitExceptValue =
      location->FindValue(location, "limit_except"); // 초기화가 필요합니다.
  if (limitExceptValue.size()) {
    if (std::find(limitExceptValue.begin(), limitExceptValue.end(),
                  mRequest.mMethod) == limitExceptValue.end()) {
      return (false);
    }
  } else if (mRequest.mMethod == "PUT" || mRequest.mMethod == "POST" ||
             mRequest.mMethod == "HEAD") {
    return (false);
  }
  return (true);
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
    return (PATH_UNKNOWN); // 디렉토리도 파일도 아닌 타입(소켓, 파이프, 심볼릭
  }
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

void Http::SetRequest(eStatusCode state, std::vector<char> &RecvBuffer) {
  if (state == SOCKET_DISCONNECTED) {
    Log(error, etc, "Socket Disconnected", *this);
    return; // disconnection();
  } else if (state == SOCKET_READ_ERROR) {
    return (ErrorHandle(state));
  }

  std::string temp(RecvBuffer.begin(), RecvBuffer.end());
  mBuffer += temp;

  mRequest.mContent.reserve(100000000);
  eStatusCode ParseState = mRequestParser.Parse(
      mRequest, mBuffer.c_str(), mBuffer.c_str() + mBuffer.size());

  if (ParseState == PARSING_ERROR) {
    mBuffer.clear();
    return (ErrorHandle(ParseState));
  } else if (ParseState == PARSING_INCOMPLETED) {
    mBuffer.clear();
    return;
  } else if (ParseState == PARSING_COMPLETED) {
    if (mRequest.mKeepAlive ==
        false) { // If Connection: close, never read again from socket
      mKeepAlive = false;
      struct kevent event;
      EV_SET(&event, mSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
      kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
    }
    mRemainingRequest++; // remaining request count

    Log(info, request, "Request", *this);
    mBuffer = mRequestParser.GetRemainingBuffer();
    HandleRequestType();
  }
}

void Http::HandleRequestType() {
  if (IsCgiRequest(*this)) {
    HandleCGIRequest();
  } else {
    HandleHTTPRequest();
  }
}

void Http::HandleCGIRequest() {
  CGIHandle(*this);
  // CGI logic
}

void Http::HandleHTTPRequest() {
  // HTTPHandle(port, *this);
  eStatusCode state = PriorityHeaders();
  if (state == REDIRECT) {
    Log(error, etc, "REDIRECT", *this);
    return; // redirect 처리
  } else if (state != PRIORITY_HEADER_OK) {
    return ErrorHandle(state);
  }

  IRequestHandler *method = Router::Routing(*this);

  // error 면 this로 가져간 http의 에러 핸들러를 호출하여 알아서 메시지를
  // 작성하도록 구현하기 정상적인 response라도 해당 메소드가 불린 시점에서는
  // 각 메서드에서 요청에 대한 처리를 하는 것을 원칙으로 작성해야할듯

  method->Handle(*this);
  // delete(method);
}

void Http::SendResponse(eStatusCode state) {
  mSendBufferRef += mResponseParser.MakeResponseMessage(*this, state);
  mRemainingRequest--;
  //   send message
  Log(info, response, "Response", *this);

  ResetAll();
}

int Http::GetPort() { return (mPort); }

int Http::GetSocket() { return (mSocket); }

std::string &Http::GetSendBuffer() { return (mSendBufferRef); }