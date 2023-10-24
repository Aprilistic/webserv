#ifndef RESPONSEMESSAGE_HPP
#define RESPONSEMESSAGE_HPP

#include "Core.hpp"
#include "Response.hpp"

#define CRLF "\r\n"
#define SP " "

class ResponseMessage {
public:
  ResponseMessage(Response &resp);
  ~ResponseMessage();

  void MakeResponseMessage(Response &resp);
  std::string GetMessage() const;
  std::vector<char> GetMessageToVector();

private:
  void setStatusLine(Response &resp);
  void setHeaderFields(Response &resp);
  void setBody(Response &resp);

private:
  std::string mMessage;
};

#endif