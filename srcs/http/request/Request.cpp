#include "Request.hpp"

Request::Request()
    : mChunked(false), mVersionMajor(0), mVersionMinor(0), mContentLength(0),
      mKeepAlive(false) {}

Request::~Request() {}

std::string Request::GetMethod() const { return mMethod; }

std::string Request::GetUri() const { return mUri; }

int Request::GetVersionMajor() const { return mVersionMajor; }

int Request::GetVersionMinor() const { return mVersionMinor; }

std::multimap<std::string, std::string> Request::GetHeaders() const {
  return mHeaders;
}

// std::vector<char> Request::GetContent() const { return mContent; }

std::string Request::GetContent() const { return mContent; }

std::string Request::GetHost() const { return mHost; }

int Request::GetContentLength() const { return mContentLength; }

std::string Request::GetContentType() const { return mContentType; }

bool Request::GetKeepAlive() const { return mKeepAlive; }

void Request::SetMethod(const std::string &method) { mMethod = method; }

void Request::SetUri(const std::string &uri) { mUri = uri; }

void Request::SetVersionMajor(int versionMajor) {
  mVersionMajor = versionMajor;
}

void Request::SetVersionMinor(int versionMinor) {
  mVersionMinor = versionMinor;
}

void Request::SetHeaders(
    const std::multimap<std::string, std::string> &headers) {

  mHeaders = headers;
}

// void Request::SetContent(const std::vector<char> &content) {
//   mContent = content;
// }

void Request::SetContent(const std::string &content) { mContent = content; }

void Request::SetHost(const std::string &host) { mHost = host; }

void Request::SetContentLength(int contentLength) {
  mContentLength = contentLength;
}

void Request::SetContentType(const std::string &contentType) {
  mContentType = contentType;
}

void Request::SetKeepAlive(bool keepAlive) { mKeepAlive = keepAlive; }

std::string Request::Inspect() const {
  std::stringstream stream;
  stream << mMethod << " " << mUri << " HTTP/" << mVersionMajor << "."
         << mVersionMinor << "\n";

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
