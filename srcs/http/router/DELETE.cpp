#include "Router.hpp"

void DeleteHandler::Handle(Http &http) {
  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      http.GetPort(), http.GetRequest().mHost, http.GetRequest().mUri,
      http.GetRequest().mMethod);
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
  std::string resolvedPath = http.GetRequest().mUri; // /example/index.html
  size_t pos = resolvedPath.find(uri[0]);            // /example
  if (pos != std::string::npos) {
    resolvedPath.replace(pos, uri[0].size(), alias[0]);
  }

  // 데이터 삭제
  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY: {
    // DELETE 요청은 리소스와 관련된 메타데이터를 모두 삭제해야 함
    // 하지만 디렉토리를 삭제하는 것은 위험할 수 있어서 허용하지 않을 수 있음
    return (http.ErrorHandle(CLIENT_ERROR_METHOD_NOT_ALLOWED));
  }
  case PATH_IS_FILE: {
    // 파일 삭제 로직
    // 삭제에 성공하면 200 OK 또는 204 No Content 반환
    // 실패하면 500 Internal Server Error 반환
    if (/*파일 삭제 성공*/ true) {
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
