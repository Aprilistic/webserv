#include "Request.hpp"

Request::Request()
    : mVersionMajor(0), mVersionMinor(0), mContentLength(0), mKeepAlive(false) {
}

Request::~Request() {}

std::string Request::Inspect() const {
  std::stringstream stream;
  stream << mMethod << " " << mUri << " HTTP/" << mVersionMajor << "."
         << mVersionMinor << "\n";

  // for (std::vector<Request::HeaderItem>::const_iterator it =
  // mHeaders.begin();
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
  stream << "+ keep-alive: " << mKeepAlive << "\n";
  return stream.str();
}
