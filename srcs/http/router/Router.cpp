#include "Router.hpp"

Router::Router() {}

Router::Router(const Router &other) { (void)other; }

Router &Router::operator=(const Router &other) { return *this; }

Router::~Router() {}

IRequestHandler *Router::Routing(Http &http) {
  if (http.GetRequest().GetMethod() == "GET") {
    return (new GetHandler());
  } else if (http.GetRequest().GetMethod() == "POST") {
    return (new PostHandler());
  } else if (http.GetRequest().GetMethod() == "DELETE") {
    return (new DeleteHandler());
  }
  return (NULL);
}

GetHandler::~GetHandler() {}