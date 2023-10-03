#include "ResponseSyntax.hpp"

ResponseSyntax::ResponseSyntax()
    : versionMajor(0), versionMinor(0), keepAlive(false), statusCode(0) {}

ResponseSyntax::~ResponseSyntax() {}

std::string ResponseSyntax::inspect() const {
  std::stringstream stream;
  stream << "HTTP/" << versionMajor << "." << versionMinor << " " << statusCode
         << " " << status << "\n";

  for (std::vector<ResponseSyntax::HeaderItem>::const_iterator it =
           headers.begin();
       it != headers.end(); ++it) {
    stream << it->name << ": " << it->value << "\n";
  }

  std::string data(content.begin(), content.end());
  stream << data << "\n";
  return stream.str();
}