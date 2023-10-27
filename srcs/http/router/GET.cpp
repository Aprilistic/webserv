#include "Router.hpp"

eStatusCode GetHandler::Handle(int port, Http &http) {
  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.GetRequest().mHost, http.GetRequest().mUri);

  // NULL 인경우 -> 일치하는 location이 없고 / 도 설정되어 있지 않은 경우
  if (location == NULL) {
    return CLIENT_ERROR_NOT_FOUND; // return 값 고민
  }

  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // alias가 없는 경우 location 이 / 라면 기본 설정에 정의되어 있는 index.html
  // 그 외의 다른 location의 경우 404

  if (alias.empty()) {
    if (uri[0] == "/") {
      return CLIENT_ERROR_NOT_FOUND; // return 값 고민
    }
  }
  // alias 있는 경우
  // resolvedPath = uri(/abc/)+ alias(/var/www/wow) = /var/www/wow/abc/
  // 구한 경로가 디렉토리인지 파일인지 권한에러인지 언노운파일인지 확인하는
  // 로직
  std::string resolvedPath = http.GetRequest().mUri; // /example/index.html
  size_t pos = resolvedPath.find(uri[0]);            // /example
  if (pos != std::string::npos) {
    resolvedPath.replace(pos, uri[0].size(), alias[0]);
  }

  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY: {
    std::vector<std::string> index = location->FindValue(location, "index");
    bool found = false;
    std::string fullPath;

    for (size_t i = 0; i < index.size(); i++) {
      // configfile에 index에 있는 파일을 resolvedPath에 붙이면서 파일인지
      // 확인
      fullPath = resolvedPath + index[i]; // file
      if (http.CheckPathType(fullPath) == PATH_IS_FILE) {
        found = true;
        break;
      }
    }
    // 경로에 파일이 있다면
    if (found) {
      // fullPath에 저장된 파일 제공
      eStatusCode readStatus = http.ReadFile(fullPath);
      if (readStatus != SUCCESSFUL_OK) {
        return (readStatus);
      }

      return SUCCESSFUL_OK;
    } else {
      http.GetResponse().mFilename = "autoindex";
      // 경로에 파일이 없다면
      // autoindex 설정 확인
      std::vector<std::string> autoindex =
          location->FindValue(location, "autoindex");

      if (autoindex.empty() == false && autoindex[0] == "on") {
        // autoindex on 일때 처리 로직
        http.GetResponse().mBody = http.AutoIndex(resolvedPath);
        http.GetResponse().mStatusCode = SUCCESSFUL_OK;
        return SUCCESSFUL_OK;
      } else {
        // autoindex가 off 일때 처리 로직
        return CLIENT_ERROR_NOT_FOUND;
      }
    }
    break;
  }
  case PATH_IS_FILE: {
    eStatusCode readStatus = http.ReadFile(resolvedPath);
    if (readStatus != SUCCESSFUL_OK) {
      return (readStatus);
    }
    return SUCCESSFUL_OK;
    // 파일 탐색 실패 -> http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUND);
    // 404;
    break;
  }
  case PATH_INACCESSIBLE: {        // 권한에러
    return CLIENT_ERROR_FORBIDDEN; // 403??
  }
  default: { // 언노운 파일
    // 파일도 디렉토리도 아니면 404 확인필요
    return CLIENT_ERROR_FORBIDDEN;
  }
  }
  return SUCCESSFUL_OK;
}
