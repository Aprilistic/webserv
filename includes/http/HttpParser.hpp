#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "Core.hpp"
#include "Enum.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "RequestSyntax.hpp"
#include "Response.hpp"
#include "ResponseMessage.hpp"
#include "ResponseParser.hpp"
#include "Router.hpp"

class HttpParser {
public:
  HttpParser();
  ~HttpParser();

  eParseResult parseRequest(const std::vector<char> &buffer);
  eParseResult parseResponse(const std::vector<char> &buffer);

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