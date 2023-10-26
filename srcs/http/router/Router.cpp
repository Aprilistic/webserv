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
