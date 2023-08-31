#include "Request.hpp"

Request::Request() : versionMajor(0), versionMinor(0), keepAlive(false) {}

Request::~Request() {}

std::string Request::inspect() const {
  std::stringstream stream;
  stream << method << " " << uri << " HTTP/" << versionMajor << "."
         << versionMinor << "\n";

  for (std::vector<Request::HeaderItem>::const_iterator it = headers.begin();
       it != headers.end(); ++it) {
    stream << it->name << ": " << it->value << "\n";
  }

  std::string data(content.begin(), content.end());
  stream << data << "\n";
  stream << "+ keep-alive: " << keepAlive << "\n";
  return stream.str();
}
