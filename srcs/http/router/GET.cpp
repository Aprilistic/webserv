#include "Router.hpp"

void GetHandler::Handle(Http &http) {

  Node *location = Common::mConfigMap->GetConfigNode(
      http.GetPort(), http.GetRequest().mHost, http.GetRequest().mUri,
      http.GetRequest().mMethod);

  // NULL 인경우 -> 일치하는 location이 없고 / 도 설정되어 있지 않은 경우
  if (location == NULL) {
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }

  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // alias가 없는 경우 location 이 / 라면 기본 설정에 정의되어 있는 index.html
  // 그 외의 다른 location의 경우 404

  if (alias.empty()) {
    if (uri[0] == "/") {
      return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
  }
  // alias 있는 경우
  // resolvedPath = uri(/abc/)+ alias(/var/www/wow) = /var/www/wow/abc/
  // 구한 경로가 디렉토리인지 파일인지 권한에러인지 언노운파일인지 확인하는
  // 로직
  std::string resolvedPath = http.GetRequest().mUri; // /example/index.html

  size_t pos = resolvedPath.find(uri[0]); // /example
  if (pos != std::string::npos) {
    // resolvedPath = alias + uri
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
      fullPath = resolvedPath + "/" + index[i]; // file
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
        return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
      }

      break;
    } else {
      http.GetResponse().mFilename = "autoindex";
      // 경로에 파일이 없다면
      // autoindex 설정 확인
      std::vector<std::string> autoindex =
          location->FindValue(location, "autoindex");

      if (autoindex.empty() == false && autoindex[0] == "on") {
        // autoindex on 일때 처리 로직
        http.GetResponse().mBody = autoIndex(resolvedPath);
        http.GetResponse().mStatusCode = SUCCESSFUL_OK;
        break;
      } else {
        // autoindex가 off 일때 처리 로직
        return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
      }
    }
    break;
  }
  case PATH_IS_FILE: {
    eStatusCode readStatus = http.ReadFile(resolvedPath);
    if (readStatus != SUCCESSFUL_OK) {
      return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
    // 파일 탐색 실패 -> http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUN, socketD);
    // 404;
    break;
  }
  case PATH_INACCESSIBLE: { // 권한에러
    return (http.ErrorHandle(CLIENT_ERROR_FORBIDDEN));
  }
  case PATH_NOT_FOUND: {
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  default: { // 언노운 파일
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  }

  http.SendResponse(SUCCESSFUL_OK);
}

std::string GetHandler::autoIndex(const std::string &path) {
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
