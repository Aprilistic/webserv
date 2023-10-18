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
      request.getRequest().mUri.substr(request.getRequest().mUri.size() - 4) == ".bla") {
    return (true);
  }

  // uri에 특정 location이 들어오지 않고, 확장자도 주어지지 않았는데 cgi로
  // 처리해야한다는 내용이 configfile에 들어있을 수 있는 지 확인 필요

  return (false);
}

// ex
eStatusCode GetHandler::handle(int port, Http& http) {
  eStatusCode res;
  http.getResponse().mStatusCode = 200;
  http.getResponse().mBody = "GET request received for URI: " + http.getRequest().mUri;
  std::cout << http.getResponse().mStatusCode << std::endl;
  std::cout << http.getResponse().mBody << std::endl;

  Node* location = Common::mConfigMap->GetConfigNode(port, http.getRequest().mHost, http.getRequest().mUri);

  (void)location;
  // after logic
  return res;
}

eStatusCode PostHandler::handle(int port, Http& http) {
  eStatusCode res;
  http.getResponse().mStatusCode = 201;

  http.getResponse().mBody = "POST request received with content: " +
              std::string(http.getRequest().mContent.begin(), http.getRequest().mContent.end());

  return res;
}

eStatusCode DeleteHandler::handle(int port, Http& http) {
  eStatusCode res;
  http.getResponse().mStatusCode = 200;

  http.getResponse().mBody = "DELETE request received for URI: " + http.getRequest().mUri;

  return res;
}

eStatusCode PutHandler::handle(int port, Http& http) {
  eStatusCode res;
  http.getResponse().mStatusCode = 200;

  http.getResponse().mBody = "PUT request received with content: " +
              std::string(http.getRequest().mContent.begin(),http.getRequest().mContent.end());

  return res;
}

eStatusCode CgiHandler::handle(int port, Http& http) {
  eStatusCode res;

  res = CLIENT_ERROR_BAD_REQUEST;
  if (http.getRequest().mMethod == "GET") {

  } else if (http.getRequest().mMethod == "POST") {
  }
  
  return res;
}
