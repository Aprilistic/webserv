#include "Router.hpp"

eStatusCode PostHandler::Handle(int port, Http &http) {
  // 요청 데이터 파싱
  std::string requestData(http.GetRequest().mContent.begin(),
                          http.GetRequest().mContent.end());

  // check if there is chunked data
  bool isChunked = false;
  std::multimap<std::string, std::string>::iterator it =
      http.GetRequest().mHeaders.find("Transfer-Encoding");
  if (it != http.GetRequest().mHeaders.end() && it->second == "chunked") {
    isChunked = true;
  }

  // 데이터가 없는 경우 400 Bad Request 반환
  if (requestData.empty() && isChunked == false) {
    std::cout << "400 Bad Request" << std::endl;
    return CLIENT_ERROR_BAD_REQUEST;
  }

  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.GetRequest().mHost, http.GetRequest().mUri);
  if (location == NULL) {
    return CLIENT_ERROR_NOT_FOUND;
  }

  // location 정보에서 alias, uri 정보 가져오기
  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // file path 구하기
  if (alias.empty()) {
    if (uri[0] == "/") {
      // nginx는 404 Not found 반환
      return CLIENT_ERROR_NOT_FOUND;
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
    // 디렉토리에 대한 POST 요청 처리 (예: 데이터베이스에 데이터 저장)
    eStatusCode writeStatus =
        http.WriteFile(resolvedPath, requestData, PATH_IS_DIRECTORY);
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.GetResponse().mStatusCode = writeStatus; // 새 리소스가 생성된 경우AA
    http.GetResponse().mStatus = http.GetStatusMessage(writeStatus);
    return writeStatus;
  }
  case PATH_IS_FILE: {
    // 파일에 대한 POST 요청 처리 (예: 파일에 데이터 추가)
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.GetResponse().mStatusCode =
        SUCCESSFUL_OK; // 리소스가 성공적으로 수정된 경우
    return SUCCESSFUL_OK;
  }
  case PATH_INACCESSIBLE: {
    return CLIENT_ERROR_FORBIDDEN;
  }
  default: {
    return CLIENT_ERROR_NOT_FOUND;
  }
  }
}
