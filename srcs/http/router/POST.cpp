#include "Router.hpp"

void PostHandler::Handle(Http &http) {
  // 요청 데이터 파싱
  std::string content = http.GetRequest().GetContent();
  std::string requestData(content.begin(), content.end());

  // check if there is chunked data
  bool isChunked = false;
  std::multimap<std::string, std::string> headers =
      http.GetRequest().GetHeaders();
  std::multimap<std::string, std::string>::iterator it =
      headers.find("Transfer-Encoding");
  if (it != headers.end() && it->second == "chunked") {
    isChunked = true;
  }

  // 데이터가 없는 경우 400 Bad Request 반환
  if (requestData.empty() && isChunked == false) {
    Log(warn, "PostHandler: requestData is empty");
    return (http.ErrorHandle(CLIENT_ERROR_BAD_REQUEST));
  }

  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      http.GetPort(), http.GetRequest().GetHost(), http.GetRequest().GetUri());
  if (location == NULL) {
    Log(warn, "PostHandler: location is NULL");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }

  // location 정보에서 alias, uri 정보 가져오기
  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // file path 구하기
  std::string resolvedPath = http.GetRequest().GetUri();
  if (alias.empty()) {
    if (uri[0] == "/") {
      Log(warn, "PostHandler: uri is /");
      return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
  } else {
    size_t pos = resolvedPath.find(uri[0]);
    if (pos != std::string::npos) {
      resolvedPath.replace(pos, uri[0].size(), alias[0]);
    }
  }

  // 데이터 처리
  eStatusCode status;
  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY: {
    status = http.WriteFile(resolvedPath, requestData, PATH_IS_DIRECTORY);
    break;
  }
  case PATH_IS_FILE: {
    status = http.WriteFile(resolvedPath, requestData, PATH_IS_FILE);
    break;
  }
  case PATH_INACCESSIBLE: {
    Log(warn, "PostHandler: PATH_INACCESSIBLE");
    return (http.ErrorHandle(CLIENT_ERROR_FORBIDDEN));
  }
  case PATH_NOT_FOUND: {
    Log(warn, "PostHandler: PATH_NOT_FOUND");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  default: {
    Log(warn, "PostHandler: default");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  }

  http.GetResponse().SetBody("<a href=\"" + resolvedPath + "\">Your resource</a>");
  http.GetResponse().InsertHeader("Content-Type", "text/html");
  http.SendResponse(status);
}
