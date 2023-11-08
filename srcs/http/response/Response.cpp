#include "Response.hpp"

Response::Response()
    : mVersionMajor(0), mVersionMinor(0), mKeepAlive(false), mStatusCode(0) {}

Response::~Response() {}

int Response::GetVersionMajor() const { return mVersionMajor; }

int Response::GetVersionMinor() const { return mVersionMinor; }

unsigned int Response::GetStatusCode() const { return mStatusCode; }

std::string Response::GetStatus() const { return mStatus; }

std::multimap<std::string, std::string> Response::GetHeaders() const {
  return mHeaders;
}

std::string Response::GetBody() const { return mBody; }

bool Response::GetKeepAlive() const { return mKeepAlive; }

void Response::SetVersionMajor(int versionMajor) {
  mVersionMajor = versionMajor;
}

void Response::SetVersionMinor(int versionMinor) {
  mVersionMinor = versionMinor;
}

void Response::SetStatusCode(unsigned int statusCode) {
  mStatusCode = statusCode;
}

void Response::SetStatus(const std::string &status) { mStatus = status; }

void Response::SetHeaders(
    const std::multimap<std::string, std::string> &headers) {
  mHeaders = headers;
}

void Response::SetBody(const std::string &body) { mBody = body; }

void Response::SetKeepAlive(bool keepAlive) { mKeepAlive = keepAlive; }

std::string Response::Inspect() const {
  std::stringstream stream;
  stream << "HTTP/" << mVersionMajor << "." << mVersionMinor << " "
         << mStatusCode << " " << mStatus << "\n";

  for (std::multimap<std::string, std::string>::const_iterator it =
           mHeaders.begin();
       it != mHeaders.end(); ++it) {
    stream << it->first << ": " << it->second << "\n";
  }

  // stream << mBody << "\n";
  return stream.str();
}