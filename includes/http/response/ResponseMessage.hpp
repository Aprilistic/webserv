#ifndef RESPONSEMESSAGE_HPP
#define RESPONSEMESSAGE_HPP

#include "Core.hpp"
#include "Response.hpp"
#include "Enum.hpp"

#define CRLF "\r\n"
#define SP " "

class ResponseParser {
public:
  ResponseParser(Response &resp);
  ~ResponseParser();

  void MakeResponseMessage(Response &resp);

  std::string MakeResponseMessage(Http& http);
  
  std::string GetMessage() const;
  std::vector<char> GetMessageToVector();
  std::string GetStatusMessage(eStatusCode errorStatus);

  std::string GetFileType();
  void MakeMandatoryHeaders();
private:
  void setStatusLine(Response &resp);
  void setHeaderFields(Response &resp);
  void setBody(Response &resp);

private:
  std::string mMessage;
};

// 1. Response Headers
// 2. Make message
#endif