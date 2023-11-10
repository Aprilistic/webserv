#include "Router.hpp"

void DeleteHandler::Handle(Http &http) {
  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      http.GetPort(), http.GetRequest().GetHost(), http.GetRequest().GetUri(),
      http.GetRequest().GetMethod());
  if (location == NULL) {
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }

  // location 정보에서 alias, uri 정보 가져오기
  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // file path 구하기
  if (alias.empty()) {
    if (uri[0] == "/") {
      // 어떤 동작하는지 찾아봐야 함
      return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
  }
  std::string resolvedPath = http.GetRequest().GetUri();
  size_t pos = resolvedPath.find(uri[0]);
  if (pos != std::string::npos) {
    resolvedPath.replace(pos, uri[0].size(), alias[0]);
  }

  // 데이터 삭제
  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY:
  case PATH_IS_FILE: {
    if (std::remove(resolvedPath.c_str()) == 0) {
      return;
    } else {
      return (http.ErrorHandle(SERVER_ERROR_INTERNAL_SERVER_ERROR));
    }
  }
  case PATH_INACCESSIBLE: {
    return (http.ErrorHandle(CLIENT_ERROR_FORBIDDEN));
  }
  case PATH_NOT_FOUND: {
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  default: {
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  }

  http.SendResponse(SUCCESSFUL_OK);
}
