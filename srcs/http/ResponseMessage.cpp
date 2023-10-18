#include "ResponseMessage.hpp"

ResponseMessage::ResponseMessage(Response &resp) { MakeResponseMessage(resp); }

ResponseMessage::~ResponseMessage() {}

void ResponseMessage::MakeResponseMessage(Response &resp) {
  setStatusLine(resp);
  setHeaderFields(resp);
  mMessage += CRLF;
  setBody(resp);
}

std::string ResponseMessage::getMessage() const { return mMessage; }

std::vector<char> ResponseMessage::getMessageToVector() {
  std::vector<char> message;
  for (std::string::iterator it = mMessage.begin(); it != mMessage.end();
       ++it) {
    message.push_back(*it);
  }
  return message;
}

void ResponseMessage::setStatusLine(Response &resp) {
  mMessage += "HTTP/";
  mMessage += std::to_string(resp.mVersionMajor);
  mMessage += ".";
  mMessage += std::to_string(resp.mVersionMinor);
  mMessage += SP;
  mMessage += std::to_string(resp.mStatusCode);
  mMessage += SP;
  mMessage += resp.mStatus + CRLF;
}

void ResponseMessage::setHeaderFields(Response &resp) {
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

void ResponseMessage::setBody(Response &resp) {
  for (std::vector<char>::iterator it = resp.mContent.begin();
       it != resp.mContent.end(); ++it) {
    mMessage += *it;
  }
}
