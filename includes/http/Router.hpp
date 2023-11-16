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
  static IRequestHandler *Routing(Http &http);

private:
  Router();
  ~Router();
  Router(const Router &other);
  Router &operator=(const Router &other);
};

class GetHandler : public IRequestHandler {
public:
  ~GetHandler();

  virtual void Handle(Http &http);

private:
  enum e_autoindex { AUTOINDEX_ALIGN_FILE_NAME, AUTOINDEX_ALIGN_FILE_SIZE };

  void alignAutoIndex(std::string &body, size_t minus_len, int to_align);
  std::string autoIndex(const std::string &path, const std::string uri);
};

class PostHandler : public IRequestHandler {
public:
  virtual void Handle(Http &http);
};

class DeleteHandler : public IRequestHandler {
public:
  virtual void Handle(Http &http);
};

#endif