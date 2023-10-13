#include "Router.hpp"

Router::Router() {}

Router::Router(const Router &other) { (void)other; }

Router &Router::operator=(const Router &other) { return *this; }

Router::~Router() {}

IRequestHandler *Router::Routing(Request &request) {
  // ex
  if (request.mMethod == "GET") {
    return (new GetHandler());
  } else if (request.mMethod == "POST") {
    return (new PostHandler());
  } else if (request.mMethod == "DELETE") {
    return (new DeleteHandler());
  } else if (request.mMethod == "PUT") {
    return (new PutHandler());
  } else if (request.mMethod == "CGI") {
    return (new CgiHandler());
  }

  return (NULL);
}

// ex
Response GetHandler::handle(Request &request) {
  Response res;
  res.mStatusCode = 200;

  res.mBody = "GET request received for URI: " + request.mUri;

  return res;
}

Response PostHandler::handle(Request &request) {
  Response res;
  res.mStatusCode = 201;

  res.mBody = "POST request received with content: " +
              std::string(request.mContent.begin(), request.mContent.end());

  return res;
}

Response DeleteHandler::handle(Request &request) {
  Response res;
  res.mStatusCode = 200;

  res.mBody = "DELETE request received for URI: " + request.mUri;

  return res;
}

Response PutHandler::handle(Request &request) {
  Response res;
  res.mStatusCode = 200;

  res.mBody = "PUT request received with content: " +
              std::string(request.mContent.begin(), request.mContent.end());

  return res;
}

Response CgiHandler::handle(Request &request) {
  Response res;
  res.mStatusCode = 200;

  res.mBody = "CGI request received for URI: " + request.mUri;

  return res;
}
