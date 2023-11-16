#include "Http.hpp"
#include "ResponseParser.hpp"

ResponseParser::ResponseParser() {}

ResponseParser::~ResponseParser() {}

std::string ResponseParser::MakeResponseMessage(Http &http, eStatusCode state) {
  // set mandatory response message
  setResponse(http, state);
  // set response message to std::string
  setMessage(http.GetResponse());
  return GetMessage();
}

void ResponseParser::setMessage(Response &resp) {
  setStatusLine(resp);
  setHeaderFields(resp);
  mMessage += CRLF;
  setBody(resp);
}

std::string const &ResponseParser::GetMessage() const { return mMessage; }

std::vector<char> ResponseParser::GetMessageToVector() {
  std::vector<char> message;
  for (std::string::iterator it = mMessage.begin(); it != mMessage.end();
       ++it) {
    message.push_back(*it);
  }
  return message;
}

void ResponseParser::setStatusLine(Response &resp) {
  mMessage += "HTTP/";
  mMessage += ToString(resp.GetVersionMajor());
  mMessage += ".";
  mMessage += ToString(resp.GetVersionMinor());
  mMessage += SP;
  mMessage += ToString(resp.GetStatusCode());
  mMessage += SP;
  mMessage += resp.GetStatus() + CRLF;
}

void ResponseParser::setHeaderFields(Response &resp) {
  std::multimap<std::string, std::string> headers = resp.GetHeaders();
  for (std::multimap<std::string, std::string>::iterator it = headers.begin();
       it != headers.end(); ++it) {
    mMessage += it->first;
    mMessage += ": ";
    mMessage += it->second;
    mMessage += CRLF;
  }
}

void ResponseParser::setBody(Response &resp) { mMessage += resp.GetBody(); }
