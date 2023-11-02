#include "Router.hpp"

void PutHandler::Handle(int port, Http &http, int socket) {
  // 요청 데이터 파싱
  std::string requestData(http.GetRequest().mContent.begin(),
                          http.GetRequest().mContent.end());

  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.GetRequest().mHost, http.GetRequest().mUri);
  if (location == NULL) {
    return (http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUND, socket));
  }

  // location 정보에서 alias, uri 정보 가져오기
  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // file path 구하기
  if (alias.empty()) {
    if (uri[0] == "/") {
      // nginx는 404 Not found 반환
      return (http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUND, socket));
    }
  }
  std::string resolvedPath = http.GetRequest().mUri; // /example/index.html
  size_t pos = resolvedPath.find(uri[0]);            // /example
  if (pos != std::string::npos) {
    resolvedPath.replace(pos, uri[0].size(), alias[0]);
  }

  // 데이터 처리
  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY: {
    // PUT에 대한 요청이 들어오면 405 Method Not Allowed 반환할 수 있음
    return (http.ErrorHandle(port, CLIENT_ERROR_METHOD_NOT_ALLOWED, socket));
  }
    case PATH_IS_FILE: {
    // 파일에 대한 PUT 요청 처리 (예: 파일에 데이터 추가)
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.GetResponse().mBody =
        "Data successfully processed and resource modified at URI: " +
        http.GetRequest().mUri;
    break ;
  }
  case PATH_INACCESSIBLE: {
    return (http.ErrorHandle(port, CLIENT_ERROR_FORBIDDEN, socket));
  }
  case PATH_NOT_FOUND: {
    return (http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUND, socket));
  }
  default: {
    return (http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUND, socket));
  }
  }

  http.SendResponse(SUCCESSFUL_OK, port, socket);
}
