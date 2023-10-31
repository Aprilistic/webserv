#include "ResponseParser.hpp"
#include "Http.hpp"

ResponseParser::ResponseParser() {}

ResponseParser::~ResponseParser() {}

std::string ResponseParser::MakeResponseMessage(Http& http, eStatusCode state)
{
  //set mandatory response message
  setResponse(http, state);
  //set response message to std::string
  setMessage(http.GetResponse());
  return GetMessage();
}

void ResponseParser::setMessage(Response &resp) {
  setStatusLine(resp);
  setHeaderFields(resp);
  mMessage += CRLF;
  setBody(resp);
}

std::string ResponseParser::GetMessage() const { return mMessage; }

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
  mMessage += std::to_string(resp.mVersionMajor);
  mMessage += ".";
  mMessage += std::to_string(resp.mVersionMinor);
  mMessage += SP;
  mMessage += std::to_string(resp.mStatusCode);
  mMessage += SP;
  mMessage += resp.mStatus + CRLF;
}

void ResponseParser::setHeaderFields(Response &resp) {
  // for (std::vector<Response::HeaderItem>::iterator it =
  // resp.mHeaders.begin();
  //      it != resp.mHeaders.end(); ++it) {
  //   mMessage += it->name;
  //   mMessage += ": ";
  //   mMessage += it->value;
  //   mMessage += CRLF;
  // }
  for (std::multimap<std::string, std::string>::iterator it =
           resp.mHeaders.begin();
       it != resp.mHeaders.end(); ++it) {
    mMessage += it->first;
    mMessage += ": ";
    mMessage += it->second;
    mMessage += CRLF;
  }
}

void ResponseParser::setBody(Response &resp) { mMessage += resp.mBody; }
