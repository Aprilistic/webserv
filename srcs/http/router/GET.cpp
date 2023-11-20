#include "Router.hpp"

void GetHandler::Handle(Http &http) {

  Node *location = Common::mConfigMap->GetConfigNode(
      http.GetPort(), http.GetRequest().GetHost(), http.GetRequest().GetUri());

  // NULL 인경우 -> 일치하는 location이 없고 / 도 설정되어 있지 않은 경우
  if (location == NULL) {
    Log(warn, "GetHandler: location is NULL");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }

  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // alias가 없는 경우 location 이 / 라면 기본 설정에 정의되어 있는 index.html
  // 그 외의 다른 location의 경우 404

  std::string resolvedPath = http.GetRequest().GetUri();
  if (alias.empty()) {
    if (uri[0] == "/") {
      return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
  } else {
    size_t pos = resolvedPath.find(uri[0]);
    if (pos != std::string::npos) {
      resolvedPath.replace(pos, uri[0].size(), alias[0]);
    }
  }

  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY: {

    std::vector<std::string> index = location->FindValue(location, "index");
    bool found = false;
    std::string fullPath;

    for (size_t i = 0; i < index.size(); i++) {
      // configfile에 index에 있는 파일을 resolvedPath에 붙이면서 파일인지 확인
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
        Log(warn, "GetHandler: read error");
        return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
      }

      break;
    } else {
      http.GetResponse().SetFilename("autoindex");
      // 경로에 파일이 없다면 autoindex 설정 확인
      std::vector<std::string> autoindex =
          location->FindValue(location, "autoindex");

      if (autoindex.empty() == false && autoindex[0] == "on") {
        // autoindex on 일때 처리 로직
        http.GetResponse().SetBody(
            autoIndex(resolvedPath, http.GetRequest().GetUri()));
        http.GetResponse().SetStatusCode(SUCCESSFUL_OK);
        break;
      } else {
        // autoindex가 off 일때 처리 로직
        Log(warn, "GetHandler: autoindex off");
        return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
      }
    }
    break;
  }
  case PATH_IS_FILE: {
    eStatusCode readStatus = http.ReadFile(resolvedPath);
    if (readStatus != SUCCESSFUL_OK) {
      Log(warn, "GetHandler: read error");
      return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
    break;
  }
  case PATH_INACCESSIBLE: { // 권한에러
    Log(warn, "GetHandler: PATH_INACCESSIBLE");
    return (http.ErrorHandle(CLIENT_ERROR_FORBIDDEN));
  }
  case PATH_NOT_FOUND: {
    Log(warn, "GetHandler: PATH_NOT_FOUND");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  default: { // 언노운 파일
    Log(warn, "GetHandler: default");
    return (http.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }
  }

  http.SendResponse(SUCCESSFUL_OK);
}

void GetHandler::alignAutoIndex(std::string &body, size_t minus_len,
                                int to_align) {
  const int size[] = {51, 20};
  int alignSize = size[to_align] - minus_len;

  while (alignSize) {
    body += " ";
    --alignSize;
  }
}

std::string GetHandler::autoIndex(const std::string &path,
                                  const std::string uri) {
  /* 각 파일에 대한 내용을 추가 */
  struct stat file_info;
  DIR *dirStream;
  struct dirent *dirInfo;
  std::string fileName;
  std::string filePath;
  std::string fileSize;
  std::string body;
  static char modifiedTime[18];

  // 제목 추가
  body += "<html>\n"
          "<head><title>Index of " +
          uri +
          "</title></head>\n"
          "<body>\n"
          "<h1>Index of " +
          uri +
          "</h1>\n"
          "<hr><pre><a href=\"../\">../</a>\n";

  dirStream = ::opendir(path.c_str());
  for (int i = 0; i < 2; ++i)
    dirInfo = ::readdir(dirStream);

  while ((dirInfo = ::readdir(dirStream)) != NULL) {
    fileName = dirInfo->d_name;
    filePath = path + "/" + fileName;

    if (stat(filePath.c_str(), &file_info) == 0) {
      if (file_info.st_mode & S_IFDIR)
        fileName += '/';

      // 파일 링크 추가
      body += "<a href=\"" + fileName + "\">" + fileName + "</a>";

      // 공백 추가 + 생성 시간 추가
      alignAutoIndex(body, fileName.length(), AUTOINDEX_ALIGN_FILE_NAME);
      strftime(modifiedTime, sizeof(modifiedTime), "%d-%b-%Y %R",
               gmtime(&file_info.st_birthtimespec.tv_sec));
      body += modifiedTime;

      // 공백 추가 + 파일 크기 추가
      if (file_info.st_mode & S_IFDIR)
        fileSize = "-";
      else
        fileSize = ToString(file_info.st_size);
      alignAutoIndex(body, fileSize.length(), AUTOINDEX_ALIGN_FILE_SIZE);
      body += fileSize;

      body += "\n";
    }
  }
  ::closedir(dirStream);

  body += "</pre><hr></body>\n"
          "</html>\n";

  return (body);
}
