#include "Router.hpp"

Router::Router() {}

Router::Router(const Router &other) { (void)other; }

Router &Router::operator=(const Router &other) { return *this; }

Router::~Router() {}

IRequestHandler *Router::Routing(Http &http) {
  // CGI 처리
  if (IsCgiRequest(http)) {
    return (new CgiHandler());
  }
  //   다른 요청 처리
  if (http.GetRequest().mMethod == "GET") {
    return (new GetHandler());
  } else if (http.GetRequest().mMethod == "POST") {
    return (new PostHandler());
  } else if (http.GetRequest().mMethod == "DELETE") {
    return (new DeleteHandler());
  } else if (http.GetRequest().mMethod == "PUT") {
    return (new PutHandler());
  }
  return (NULL);
}

bool Router::IsCgiRequest(Http &request) {
  // uri에  이러한 경로가 있다면 무조건  cgi에서 처리된다는 가정
  if (request.GetRequest().mUri.find("/cgi-bin/") != std::string::npos) {
    return (true);
  }

  // 클라이언트가 확장자를 지정해준 경우 이 파일은 무조건 cgi에서 처리된다는
  // 가정
  if (request.GetRequest().mUri.size() > 4 &&
      request.GetRequest().mUri.substr(request.GetRequest().mUri.size() - 4) ==
          ".bla") {
    return (true);
  }

  // uri에 특정 location이 들어오지 않고, 확장자도 주어지지 않았는데 cgi로
  // 처리해야한다는 내용이 configfile에 들어있을 수 있는 지 확인 필요

  return (false);
}

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

eStatusCode PostHandler::Handle(int port, Http &http) {
  // 요청 데이터 파싱
  std::string requestData(http.GetRequest().mContent.begin(),
                          http.GetRequest().mContent.end());

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
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.GetResponse().mStatusCode = SUCCESSFUL_OK; // 새 리소스가 생성된 경우AA
    http.GetResponse().mBody =
        "Data successfully processed and resource created at URI: " +
        http.GetRequest().mUri;
    return SUCCESSFUL_CREATERD;
  }
  case PATH_IS_FILE: {
    // 파일에 대한 POST 요청 처리 (예: 파일에 데이터 추가)
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.GetResponse().mStatusCode =
        SUCCESSFUL_OK; // 리소스가 성공적으로 수정된 경우
    http.GetResponse().mBody =
        "Data successfully processed and resource modified at URI: " +
        http.GetRequest().mUri;
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

eStatusCode DeleteHandler::Handle(int port, Http &http) {
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
      // 어떤 동작하는지 찾아봐야 함
      return CLIENT_ERROR_NOT_FOUND;
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
    return CLIENT_ERROR_METHOD_NOT_ALLOWED;
  }
  case PATH_IS_FILE: {
    // 파일 삭제 로직
    // 삭제에 성공하면 200 OK 또는 204 No Content 반환
    // 실패하면 500 Internal Server Error 반환
    if (/*파일 삭제 성공*/ true) {
      http.GetResponse().mStatusCode = SUCCESSFUL_OK;
      return SUCCESSFUL_OK;
    } else {
      return SERVER_ERROR_INTERNAL_SERVER_ERROR;
    }
  }
  case PATH_INACCESSIBLE: {
    return CLIENT_ERROR_FORBIDDEN;
  }
  default: {
    return CLIENT_ERROR_NOT_FOUND;
  }
  }
}

eStatusCode PutHandler::Handle(int port, Http &http) {
  // 요청 데이터 파싱
  std::string requestData(http.GetRequest().mContent.begin(),
                          http.GetRequest().mContent.end());

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
    // PUT에 대한 요청이 들어오면 405 Method Not Allowed 반환할 수 있음
    return CLIENT_ERROR_METHOD_NOT_ALLOWED;
  }
  case PATH_IS_FILE: {
    // 파일에 대한 PUT 요청 처리 (예: 파일에 데이터 추가)
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.GetResponse().mStatusCode =
        SUCCESSFUL_OK; // 리소스가 성공적으로 수정된 경우
    http.GetResponse().mBody =
        "Data successfully processed and resource modified at URI: " +
        http.GetRequest().mUri;
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

eStatusCode CgiHandler::Handle(int port, Http &http) {
  eStatusCode res;

  res = CLIENT_ERROR_BAD_REQUEST;
  if (http.GetRequest().mMethod == "GET") {

  } else if (http.GetRequest().mMethod == "POST") {
  }

  return res;
}
