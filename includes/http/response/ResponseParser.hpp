#ifndef RESPONSEPARSER_HPP
#define RESPONSEPARSER_HPP

#include "Core.hpp"
#include "Enum.hpp"

#define CRLF "\r\n"
#define SP " "

class Http;
class Response;

class ResponseParser {
public:
  ResponseParser();
  ~ResponseParser();

  std::string MakeResponseMessage(Http &http, eStatusCode state);

  std::string const &GetMessage() const;
  std::vector<char> GetMessageToVector();

private:
  // set mandatory response message
  void setResponse(Http &http, eStatusCode state);
  void setStatusLine(Http &http, eStatusCode state);
  void setMandatoryHeaderFields(Http &http);
  void setCookie(Http &http);
  std::string getStatusMessage(eStatusCode errorStatus);
  std::string getFileType(Http &http);

  // set response message to std::string
  void setMessage(Response &resp);
  void setStatusLine(Response &resp);
  void setHeaderFields(Response &resp);
  void setBody(Response &resp);

private:
  std::string mMessage;
};

#endif