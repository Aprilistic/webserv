#include "Http.hpp"

Http::Http() {}

Http::~Http() {}

eStatusCode Http::requestParser(int &port, std::vector<char> &mRecvBuffer) {
  std::string tmp(mRecvBuffer.begin(), mRecvBuffer.end());
  mBuffer += tmp;

  eStatusCode result = mRequestParser.parse(mRequest, mBuffer.c_str(),
                                            mBuffer.c_str() + mBuffer.size());

  switch (result) {
  case (ParsingCompleted):
    mBuffer = mRequestParser.getRemainingBuffer();
    return ParsingCompleted;
  case (ParsingIncompleted):
    mBuffer.clear();
    return ParsingIncompleted;
  default:
    return (ErrorHandle(port, CLIENT_ERROR_BAD_REQUEST), ERROR);
  }
}

eStatusCode Http::priorityHeaders(int &port) {
  if (CheckRedirect(port))
    return REDIRECT; // redirect path 로  response
  if (CheckClientMaxBodySize(port) == false)
    return (ErrorHandle(port, CLIENT_ERROR_CONTENT_TOO_LARGE), ERROR);
  if (CheckLimitExcept(port) == false)
    return (ErrorHandle(port, CLIENT_ERROR_METHOD_NOT_ALLOWED), ERROR);
  return PRIORITY_HEADER_OK;
}

std::string getStatusMessage(eStatusCode errorStatus) {
  switch (errorStatus) {
  case (SUCCESSFUL_OK):
    return ("OK");
  case (SUCCESSFUL_ACCEPTED):
    return ("Accepted");
  case (SUCCESSFUL_NO_CONTENT):
    return ("No Content");
  case (SUCCESSFUL_PARTIAL_CONTENT):
    return ("Partial Content");
  case (REDIRECT):
    return ("Redirect");
  case (CLIENT_ERROR_BAD_REQUEST):
    return ("Bad Request");
  case (CLIENT_ERROR_UNAUTHORIZED):
    return ("Unauthorized");
  case (CLIENT_ERROR_FORBIDDEN):
    return ("Forbidden");
  case (CLIENT_ERROR_NOT_FOUND):
    return ("Not Found");
  case (CLIENT_ERROR_METHOD_NOT_ALLOWED):
    return ("Method Not Allowed");
  case (CLIENT_ERROR_UNSUPPORTED_MEDIA_TYPE):
    return ("Unsupported Media Type");
  case (CLIENT_ERROR_RANGE_NOT_SATISFIABLE):
    return ("Range Not Satisfiable");
  case (SERVER_ERROR_INTERNAL_SERVER_ERROR):
    return ("Internal Server Error");
  case (SERVER_ERROR_NOT_IMPLEMENTED):
    return ("Not Implemented");
  case (SERVER_ERROR_BAD_GATEWAY):
    return ("Bad Gateway");
  case (SERVER_ERROR_GATEWAY_TIMEOUT):
    return ("Gateway Timeout");
  default:
    return ("Unknown");
  }
}

eStatusCode Http::setResponse(int &port) {
  IRequestHandler *handler = Router::Routing(*this);
  eStatusCode handleStatus = handler->handle(port, *this);

  // 나중에 signal 시 처리하기 위해 소멸자에 추가
  //  delete handler;
  getResponse().mStatusCode = handleStatus;
  getResponse().mStatus = getStatusMessage(handleStatus);

  switch (handleStatus) {
  case (SUCCESSFUL_OK):
    return SUCCESSFUL_OK;
  case (SUCCESSFUL_CREATERD):
    return SUCCESSFUL_CREATERD;
  case (SUCCESSFUL_ACCEPTED):
    return SUCCESSFUL_ACCEPTED;
  default:
    return (ErrorHandle(port, handleStatus), ERROR);
  }
}

void Http::ErrorHandle(int port, eStatusCode errorStatus) {

  Node *location =
      Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

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
        // errorPagePath response
        return;
      }
    }
  }
  // default error page responsㄷ
}

eStatusCode Http::ReadFile(const std::string &path) {
  getResponse().mFilename = path;
  mFd = open(path.c_str(), O_RDONLY);

  if (mFd == -1) {
    if (errno == ENOENT) {
      return (CLIENT_ERROR_NOT_FOUND);
    } else if (errno == EACCES) {
      return (CLIENT_ERROR_FORBIDDEN);
    }
  }
  // fcntl(mFd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

  char buffer[4096];
  ssize_t readSize;
  while ((readSize = read(mFd, buffer, 4096)) > 0) {
    getResponse().mBody += std::string(buffer, buffer + readSize);
  }
  if (readSize == -1) {
    close(mFd);
    return (SERVER_ERROR_INTERNAL_SERVER_ERROR);
  }

  close(mFd);
  return (SUCCESSFUL_OK); // 성공 후 반환값 뭐로 하지?
}

// eStatusCode Http::WriteFile(const std::string &path) {}
std::string Http::AutoIndex(const std::string &path) {
  DIR *dir = opendir(path.c_str());
  if (dir == NULL) {
    return ("");
  }

  std::string html = "<html><head><title>Index of " + path +
                     "</title></head><body><h1>Index of " + path +
                     "</h1><hr><pre>";

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    html += "<a href=\"" + std::string(entry->d_name) + "\">" +
            std::string(entry->d_name) + "</a><br>";
  }
  html += "</pre><hr></body></html>";

  closedir(dir);
  return (html);
}

bool Http::CheckRedirect(int port) {
  Node *location =
      Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

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

bool Http::CheckClientMaxBodySize(int port) {
  Node *location =
      Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

  std::vector<std::string> clientMaxBodySizeValues =
      location->FindValue(location, "client_max_body_size");

  if (clientMaxBodySizeValues.size()) {
    int valueSize =
        std::atoi(clientMaxBodySizeValues[0].c_str()); // overflow, k,M,G check

    std::multimap<std::string, std::string>::iterator it;
    it = mRequest.mHeaders.find("Content-Length");

    if (it != mRequest.mHeaders.end()) {
      int contentLength = std::atoi(it->second.c_str());
      if (contentLength > valueSize) {
        return (false);
      }
    }
  }
  return (true);
}

bool Http::CheckLimitExcept(int port) {
  Node *location =
      Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

  std::vector<std::string> limitExceptValue =
      location->FindValue(location, "limit_except"); // 초기화가 필요합니다.
  if (limitExceptValue.size()) {
    if (std::find(limitExceptValue.begin(), limitExceptValue.end(),
                  mRequest.mMethod) == limitExceptValue.end()) {
      return (false);
    }
  }
  return (true);
}

eStatusCode Http::CheckPathType(const std::string &path) {
  struct stat info;
  // stat(path.c_str(), &info);
  if (stat(path.c_str(), &info) != 0) {
    return (PATH_INACCESSIBLE); // 권한 에러 Cannot access path
  } else if (info.st_mode & S_IFDIR) {
    return (PATH_IS_DIRECTORY); // 디렉토리 Directory
  } else if (info.st_mode & S_IFREG) {
    return (PATH_IS_FILE); // 파일 Regular file
  } else {
    return (PATH_UNKNOWN); // 디렉토리도 파일도 아닌 타입(소켓, 파이프, 심볼릭
  }
  // 링크 등등)
}

// std::vector<char>& Http::parseResponse(Response response)
// {
// }

void Http::resetRequest() { mRequest = Request(); }

void Http::resetResponse() { mResponse = Response(); }

void Http::resetBuffer() { mBuffer.clear(); }

void Http::resetRequestParser() { mRequestParser = RequestParser(); }

Request &Http::getRequest() { return mRequest; }

Response &Http::getResponse() { return mResponse; }
