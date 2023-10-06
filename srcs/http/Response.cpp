#include "Response.hpp"

Response::Response()
    : mVersionMajor(0), mVersionMinor(0), mKeepAlive(false), mStatusCode(0) {}

Response::~Response() {}

std::string Response::inspect() const {
  std::stringstream stream;
  stream << "HTTP/" << mVersionMajor << "." << mVersionMinor << " "
         << mStatusCode << " " << mStatus << "\n";

  // for (std::vector<Response::HeaderItem>::const_iterator it =
  //          mHeaders.begin();
  //      it != mHeaders.end(); ++it) {
  //   stream << it->name << ": " << it->value << "\n";
  // }
  for (std::multimap<std::string, std::string>::const_iterator it =
           mHeaders.begin();
       it != mHeaders.end(); ++it) {
    stream << it->first << ": " << it->second << "\n";
  }

  std::string data(mContent.begin(), mContent.end());
  stream << data << "\n";
  return stream.str();
}