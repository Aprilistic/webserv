#include "Router.hpp"

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

eStatusCode CgiHandler::Handle(int port, Http &http) {
  eStatusCode res;

  res = CLIENT_ERROR_BAD_REQUEST;
  if (http.GetRequest().mMethod == "GET") {

  } else if (http.GetRequest().mMethod == "POST") {
  }

  return res;
}