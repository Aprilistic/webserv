#ifndef HTTP_HPP
#define HTTP_HPP

#include "Core.hpp"
#include "Enum.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "RequestSyntax.hpp"
#include "Response.hpp"
#include "ResponseMessage.hpp"
#include "ResponseParser.hpp"
#include "Router.hpp"

class Http {
public:
  Http();
  ~Http();

  eStatusCode parseRequest(const std::vector<char> &buffer);
  std::vector<char> &parseResponse(Response response);
//   void ErrorResponse(eStatusCode errorStatus);
  void resetRequest(void);
  void resetResponse(void);
  Request &getRequest(void);
  // Respone &getResponse(void);
private:
  Request mRequest;
  Response mResponse;
  std::string mBuffer;
};

#endif