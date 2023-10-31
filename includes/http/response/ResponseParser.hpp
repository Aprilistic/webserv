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

  std::string MakeResponseMessage(Http& http, eStatusCode state);
  // void MakeResponseMessage(Response &resp);
  
  std::string GetMessage() const;
  std::vector<char> GetMessageToVector();

private:
  //Set Response message base RFC
  void makeMandatoryHeaders(Http& http);
  std::string getStatusMessage(eStatusCode errorStatus);
  std::string getFileType(Http& http);

  //Make message
  void setMessage(Response &resp);
  void setStatusLine(Response &resp);
  void setHeaderFields(Response &resp);
  void setBody(Response &resp);
private:
  std::string mMessage;
};

#endif