#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

#include "Http.hpp"

class Http;

enum eHandleStatus { INCOMPLETED = 0, COMPLETED = 1 };

class IRequestHandler {
public:
  virtual eStatusCode Handle(int port, Http &http) = 0;
private:
};

#endif