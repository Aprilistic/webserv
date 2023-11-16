#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

#include "Http.hpp"

class Http;

enum eHandleStatus { INCOMPLETED = 0, COMPLETED = 1 };

class IRequestHandler {
public:
  virtual void Handle(Http &http) = 0;
  virtual ~IRequestHandler() {}
};

#endif