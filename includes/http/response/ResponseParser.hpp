#ifndef RESPONSEPARSER_HPP
#define RESPONSEPARSER_HPP

#include "Core.hpp"
#include "Http.hpp"
#include "Enum.hpp"

#define CRLF "\r\n"
#define SP " "

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

// 1. Response Headers
// http/1.1 200 OK
// Headers: asjd;fja;sdf
// Headers: asjd;fja;sdf
// Headers: asjd;fja;sdf
// bodyu
// 2. Make message
// 
#endif