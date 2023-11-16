#include "Response.hpp"

Response::Response()
    : mVersionMajor(0), mVersionMinor(0), mKeepAlive(false), mStatusCode(0) {}

Response::~Response() {}

int Response::GetVersionMajor() const { return mVersionMajor; }

int Response::GetVersionMinor() const { return mVersionMinor; }

unsigned int Response::GetStatusCode() const { return mStatusCode; }

std::string const &Response::GetStatus() const { return mStatus; }

std::multimap<std::string, std::string> const &Response::GetHeaders() const {
  return mHeaders;
}

std::string const &Response::GetBody() const { return mBody; }

bool Response::GetKeepAlive() const { return mKeepAlive; }

std::string const &Response::GetFilename() const { return mFilename; }

void Response::InsertHeader(const std::string &key, const std::string &value) {
  mHeaders.insert(std::pair<std::string, std::string>(key, value));
}

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

void Response::SetBody(const std::string &body) { mBody = body; }

void Response::SetKeepAlive(bool keepAlive) { mKeepAlive = keepAlive; }

void Response::SetFilename(const std::string &filename) {
  mFilename = filename;
}

std::string Response::Inspect() const {
  std::stringstream stream;
  stream << "HTTP/" << mVersionMajor << "." << mVersionMinor << " "
         << mStatusCode << " " << mStatus << "\n";

  for (std::multimap<std::string, std::string>::const_iterator it =
           mHeaders.begin();
       it != mHeaders.end(); ++it) {
    stream << it->first << ": " << it->second << "\n";
  }

  stream << mBody << "\n";
  return stream.str();
}