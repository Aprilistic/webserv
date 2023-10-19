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
  if (http.getRequest().mMethod == "GET") {
    return (new GetHandler());
  } else if (http.getRequest().mMethod == "POST") {
    return (new PostHandler());
  } else if (http.getRequest().mMethod == "DELETE") {
    return (new DeleteHandler());
  } else if (http.getRequest().mMethod == "PUT") {
    return (new PutHandler());
  }
  return (NULL);
}

bool Router::IsCgiRequest(Http &request) {
  // uri에  이러한 경로가 있다면 무조건  cgi에서 처리된다는 가정
  if (request.getRequest().mUri.find("/cgi-bin/") != std::string::npos) {
    return (true);
  }

  // 클라이언트가 확장자를 지정해준 경우 이 파일은 무조건 cgi에서 처리된다는
  // 가정
  if (request.getRequest().mUri.size() > 4 &&
      request.getRequest().mUri.substr(request.getRequest().mUri.size() - 4) ==
          ".bla") {
    return (true);
  }

  // uri에 특정 location이 들어오지 않고, 확장자도 주어지지 않았는데 cgi로
  // 처리해야한다는 내용이 configfile에 들어있을 수 있는 지 확인 필요

  return (false);
}

// ex
eStatusCode GetHandler::handle(int port, Http &http) {
  eStatusCode res;
  http.getResponse().mStatusCode = 200;
  http.getResponse().mBody =
      "GET request received for URI: " + http.getRequest().mUri;
  std::cout << http.getResponse().mStatusCode << std::endl;
  std::cout << http.getResponse().mBody << std::endl;

  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.getRequest().mHost, http.getRequest().mUri);

  // NULL 인경우 -> 일치하는 location이 없고 / 도 설정되어 있지 않은 경우
  if (location == NULL) {
    // 기본 설정에 정의되어 있는 index.html(Welcom nginx)로 처리하거나
    // 파일 읽기 로직
    // 404로 처리
    http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
    return CLIENT_ERROR_NOT_FOUND; // return 값 고민
  }

  std::string findLocation = "location";
  std::vector<std::string> uri = location->FindValue(location, findLocation);

  std::string findAlias = "alias";
  std::vector<std::string> alias = location->FindValue(location, findAlias);

  // alias가 없는 경우 location 이 / 라면 기본 설정에 정의되어 있는 index.html
  // 그 외의 다른 location의 경우 404
  if (alias.empty()) {
    if (uri[0] == "/")
      return SUCCESSFUL_OK; // 기본 설정에 정의되어 있는 index.html(Welcom
                            // nginx)
    else {
      http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
      return CLIENT_ERROR_NOT_FOUND; // return 값 고민
    }
  } else {
    // alias 있는 경우
    // resolvedPath = uri(/abc/)+ alias(/var/www/wow) = /var/www/wow/abc/
    // 구한 경로가 디렉토리인지 파일인지 권한에러인지 언노운파일인지 확인하는
    // 로직
    std::string resolvedPath = uri[0] + alias[0];

    switch (http.CheckPathType(resolvedPath)) {
    // 디렉토리 처리 로직
    case PATH_IS_DIRECTORY: {
      std::string findIndex = "index";
      std::vector<std::string> index = location->FindValue(location, findIndex);
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
        http.getResponse().mStatusCode = SUCCESSFUL_OK;
        return SUCCESSFUL_OK;
      } else {
        // 경로에 파일이 없다면
        // autoindex 설정 확인
        std::string findAutoIndex = "autoindex";
        std::vector<std::string> autoindex =
            location->FindValue(location, findAutoIndex);

        if (autoindex.empty() == false && autoindex[0] == "on") {
          // autoindex on 일때 처리 로직
          http.getResponse().mStatusCode = SUCCESSFUL_OK;
          return SUCCESSFUL_OK;
        } else {
          // autoindex가 off 일때 처리 로직
          http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUND);
          return CLIENT_ERROR_NOT_FOUND;
        }
      }
      break;
    }
    // 파일 처리 로직
    case PATH_IS_FILE:
      // 파일 탐색 성공 -> http.getResponse().mStatusCode = SUCCESSFUL_OK
      return SUCCESSFUL_OK;
      // 파일 탐색 실패 -> http.ErrorHandle(port, CLIENT_ERROR_FORBIDDEN); 404;
      break;
    // 권한 에러
    case PATH_INACCESSIBLE:
      http.ErrorHandle(port, CLIENT_ERROR_FORBIDDEN); // 권한에러 403 확인필요
      return CLIENT_ERROR_FORBIDDEN;
    // 언노운 타입
    default:
      http.ErrorHandle(
          port,
          CLIENT_ERROR_NOT_FOUND); // 파일도 디렉토리도 아니면 404 확인필요
      return CLIENT_ERROR_FORBIDDEN;
    }
  }
}

eStatusCode PostHandler::handle(int port, Http &http) {
  // 요청 데이터 파싱
  std::string requestData(http.getRequest().mContent.begin(),
                          http.getRequest().mContent.end());

  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.getRequest().mHost, http.getRequest().mUri);
  if (location == NULL) {
    http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
    return CLIENT_ERROR_NOT_FOUND;
  }

  // location 정보에서 alias, uri 정보 가져오기
  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // file path 구하기
  std::string resolvedPath;
  if (alias.empty()) {
    if (uri[0] == "/") {
      // nginx는 404 Not found 반환
      http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
      return CLIENT_ERROR_NOT_FOUND;
    }
  } else {
    resolvedPath = uri[0] + alias[0];
  }

  // 데이터 처리
  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY: {
    // 디렉토리에 대한 POST 요청 처리 (예: 데이터베이스에 데이터 저장)
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.getResponse().mStatusCode = SUCCESSFUL_OK; // 새 리소스가 생성된 경우
    http.getResponse().mBody =
        "Data successfully processed and resource created at URI: " +
        http.getRequest().mUri;
    return SUCCESSFUL_CREATERD;
  }
  case PATH_IS_FILE: {
    // 파일에 대한 POST 요청 처리 (예: 파일에 데이터 추가)
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.getResponse().mStatusCode =
        SUCCESSFUL_OK; // 리소스가 성공적으로 수정된 경우
    http.getResponse().mBody =
        "Data successfully processed and resource modified at URI: " +
        http.getRequest().mUri;
    return SUCCESSFUL_OK;
  }
  case PATH_INACCESSIBLE: {
    http.getResponse().mStatusCode = CLIENT_ERROR_FORBIDDEN;
    return CLIENT_ERROR_FORBIDDEN;
  }
  default: {
    http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
    return CLIENT_ERROR_NOT_FOUND;
  }
  }
}

eStatusCode DeleteHandler::handle(int port, Http &http) {
  eStatusCode res;
  http.getResponse().mStatusCode = 200;

  http.getResponse().mBody =
      "DELETE request received for URI: " + http.getRequest().mUri;

  return res;
}

eStatusCode PutHandler::handle(int port, Http &http) {
  // 요청 데이터 파싱
  std::string requestData(http.getRequest().mContent.begin(),
                          http.getRequest().mContent.end());

  // URI로 리소스 위치 확인
  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.getRequest().mHost, http.getRequest().mUri);
  if (location == NULL) {
    http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
    return CLIENT_ERROR_NOT_FOUND;
  }

  // location 정보에서 alias, uri 정보 가져오기
  std::vector<std::string> uri = location->FindValue(location, "location");
  std::vector<std::string> alias = location->FindValue(location, "alias");

  // file path 구하기
  std::string resolvedPath;
  if (alias.empty()) {
    if (uri[0] == "/") {
      // nginx는 404 Not found 반환
      http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
      return CLIENT_ERROR_NOT_FOUND;
    }
  } else {
    resolvedPath = uri[0] + alias[0];
  }

  // 데이터 처리
  switch (http.CheckPathType(resolvedPath)) {
  case PATH_IS_DIRECTORY: {
    // PUT에 대한 요청이 들어오면 405 Method Not Allowed 반환할 수 있음
    http.getResponse().mStatusCode = CLIENT_ERROR_METHOD_NOT_ALLOWED;
    return CLIENT_ERROR_METHOD_NOT_ALLOWED;
  }
  case PATH_IS_FILE: {
    // 파일에 대한 PUT 요청 처리 (예: 파일에 데이터 추가)
    // 데이터 처리 후 결과에 따라 상태 코드 설정
    http.getResponse().mStatusCode =
        SUCCESSFUL_OK; // 리소스가 성공적으로 수정된 경우
    http.getResponse().mBody =
        "Data successfully processed and resource modified at URI: " +
        http.getRequest().mUri;
    return SUCCESSFUL_OK;
  }
  case PATH_INACCESSIBLE: {
    http.getResponse().mStatusCode = CLIENT_ERROR_FORBIDDEN;
    return CLIENT_ERROR_FORBIDDEN;
  }
  default: {
    http.getResponse().mStatusCode = CLIENT_ERROR_NOT_FOUND;
    return CLIENT_ERROR_NOT_FOUND;
  }
  }
}

eStatusCode CgiHandler::handle(int port, Http &http) {
  eStatusCode res;

  res = CLIENT_ERROR_BAD_REQUEST;
  if (http.getRequest().mMethod == "GET") {

  } else if (http.getRequest().mMethod == "POST") {
  }

  return res;
}
