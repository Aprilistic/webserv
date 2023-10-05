#include "Response.hpp"

Response::Response()
    : versionMajor(0), versionMinor(0), keepAlive(false), statusCode(0) {}

Response::~Response() {}

std::string Response::inspect() const {
  std::stringstream stream;
  stream << "HTTP/" << versionMajor << "." << versionMinor << " " << statusCode
         << " " << status << "\n";

  // for (std::vector<Response::HeaderItem>::const_iterator it =
  //          headers.begin();
  //      it != headers.end(); ++it) {
  //   stream << it->name << ": " << it->value << "\n";
  // }
  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); ++it) {
    stream << it->first << ": " << it->second << "\n";
  }

  std::string data(content.begin(), content.end());
  stream << data << "\n";
  return stream.str();
}