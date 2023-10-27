#include "Http.hpp"

int Http::mFileID = 0;

Http::Http() {}

Http::~Http() {}

eStatusCode Http::setOneRequest(int &port, std::vector<char> &mRecvBuffer) {
  std::string tmp(mRecvBuffer.begin(), mRecvBuffer.end());
  mBuffer += tmp;

  // std::cout << mBuffer;
  // std::cout << RED << "|" << RESET << std::endl;
  eStatusCode result = mRequestParser.Parse(mRequest, mBuffer.c_str(),
                                            mBuffer.c_str() + mBuffer.size());
  // std::cout << CYAN << "result: " << result << RESET << std::endl;
  switch (result) {
  case (PARSING_COMPLETED):
    mBuffer = mRequestParser.GetRemainingBuffer();
    return PARSING_COMPLETED;
  case (PARSING_INCOMPLETED):
    mBuffer.clear();
    return PARSING_INCOMPLETED;
  default:
    return (ErrorHandle(port, CLIENT_ERROR_BAD_REQUEST), ERROR);
  }
}

eStatusCode Http::PriorityHeaders(int &port) {
  if (checkRedirect(port))
    return REDIRECT; // redirect path 로  response
  if (checkClientMaxBodySize(port) == false)
    return (ErrorHandle(port, CLIENT_ERROR_CONTENT_TOO_LARGE), ERROR);
  if (checkLimitExcept(port) == false)
    return (ErrorHandle(port, CLIENT_ERROR_METHOD_NOT_ALLOWED), ERROR);
  return PRIORITY_HEADER_OK;
}

std::string Http::GetStatusMessage(eStatusCode errorStatus) {
  switch (errorStatus) {
  case (SUCCESSFUL_OK):
    return ("OK");
  case (SUCCESSFUL_CREATERD):
    return ("Created");
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

eStatusCode Http::SetResponse(int &port) {
  IRequestHandler *handler = Router::Routing(*this);
  eStatusCode handleStatus = handler->Handle(port, *this);

  // 나중에 signal 시 처리하기 위해 소멸자에 추가
  //  delete handler;
  if (handleStatus == CGI) {
    return (CGI);
  }
  GetResponse().mStatusCode = handleStatus;
  GetResponse().mStatus = GetStatusMessage(handleStatus);

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

  GetResponse().mStatusCode = errorStatus;
  GetResponse().mStatus = GetStatusMessage(errorStatus);

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
  ReadFile(DEFAULT_ERROR_PAGE_PATH);
  // default error page respons
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
    std::stringstream ss;
    ss << path << mFileID;

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

bool Http::checkRedirect(int port) {
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

bool Http::checkClientMaxBodySize(int port) {
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

bool Http::checkLimitExcept(int port) {
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
  if (mRequest.mMethod == "PUT" || mRequest.mMethod == "POST" ||
      mRequest.mMethod == "HEAD") {
    return (false);
  }
  return (true);
}

eStatusCode Http::CheckPathType(const std::string &path) {
  struct stat info;
  // stat(path.c_str(), &info);
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
  // 링크 등등)
}

std::vector<char> Http::GetCGIbufferToVector() {
  std::vector<char> message;
  for (std::string::iterator it = mCGIbuffer.begin(); it != mCGIbuffer.end();
       ++it) {
    message.push_back(*it);
  }
  return message;
}

void Http::ResetRequest() { mRequest = Request(); }

void Http::ResetResponse() { mResponse = Response(); }

void Http::ResetBuffer() { mBuffer.clear(); }

void Http::ResetCGIbuffer() { mCGIbuffer.clear(); }

void Http::ResetRequestParser() { mRequestParser = RequestParser(); }

void Http::SetCGIbuffer(std::string &CGIResponseMessage) {
  mCGIbuffer = CGIResponseMessage;
}

Request &Http::GetRequest() { return mRequest; }

Response &Http::GetResponse() { return mResponse; }
