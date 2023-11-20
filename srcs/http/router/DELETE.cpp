#include "Router.hpp"

void DeleteHandler::Handle(Http &http) {
  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      http.GetPort(), http.GetRequest().GetHost(), http.GetRequest().GetUri());
  if (location == NULL) {
    Log(warn, "DeleteHandler: location is NULL");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }

  // location 정보에서 alias, uri 정보 가져오기
  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // file path 구하기
  std::string resolvedPath = http.GetRequest().GetUri();
  if (alias.empty()) {
    if (uri[0] == "/") {
      Log(warn, "DeleteHandler: uri is /");
      return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
  } else {
    size_t pos = resolvedPath.find(uri[0]);
    if (pos != std::string::npos) {
      resolvedPath.replace(pos, uri[0].size(), alias[0]);
    }
  }

  // 데이터 삭제
  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY:
  case PATH_IS_FILE: {
    if (std::remove(resolvedPath.c_str()) == 0) {
      break;
    } else {
      Log(warn, "DeleteHandler: remove error");
      return (http.ErrorHandle(SERVER_ERROR_INTERNAL_SERVER_ERROR));
    }
  }
  case PATH_INACCESSIBLE: {
    Log(warn, "DeleteHandler: PATH_INACCESSIBLE");
    return (http.ErrorHandle(CLIENT_ERROR_FORBIDDEN));
  }
  case PATH_NOT_FOUND: {
    Log(warn, "DeleteHandler: PATH_NOT_FOUND");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  default: {
    Log(warn, "DeleteHandler: default");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  }

  http.SendResponse(SUCCESSFUL_OK);
}
