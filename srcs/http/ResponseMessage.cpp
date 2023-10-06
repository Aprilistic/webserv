#include "ResponseMessage.hpp"

ResponseMessage::ResponseMessage() {}

ResponseMessage::~ResponseMessage() {}

void ResponseMessage::MakeResponseMessage(Response &resp) {
  setStatusLine(resp);
  setHeaderFields(resp);
  mMessage += CRLF;
  setBody(resp);
}

std::string ResponseMessage::getMessage() const { return mMessage; }

void ResponseMessage::setStatusLine(Response &resp) {
	mMessage += "HTTP/1.1";
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
