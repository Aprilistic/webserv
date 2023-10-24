#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Common.hpp"
#include "Core.hpp"
#include "IRequestHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <sys/stat.h>

class Http;
class Request;
class Router {
public:
  // static IRequestHandler* Routing(Request& request);
  static IRequestHandler *Routing(Http &http);
  static bool IsCgiRequest(Http &http);

private:
  Router();
  ~Router();
  Router(const Router &other);
  Router &operator=(const Router &other);
};

class GetHandler : public IRequestHandler {
public:
  virtual eStatusCode Handle(int port, Http &http);
};

class PostHandler : public IRequestHandler {
public:
  virtual eStatusCode Handle(int port, Http &http);
};

class DeleteHandler : public IRequestHandler {
public:
  virtual eStatusCode Handle(int port, Http &http);
};

class PutHandler : public IRequestHandler {
public:
  virtual eStatusCode Handle(int port, Http &http);
};
class CgiHandler : public IRequestHandler {
public:
  virtual eStatusCode Handle(int port, Http &http);
};

#endif