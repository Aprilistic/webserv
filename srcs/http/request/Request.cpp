#include "Request.hpp"

Request::Request()
    : mChunked(false), mVersionMajor(0), mVersionMinor(0), mContentLength(0),
      mKeepAlive(false) {
  mContent.reserve(100000000);
}

Request::~Request() {}

bool Request::GetChunked() const { return mChunked; }

bool Request::GetKeepAlive() const { return mKeepAlive; }

int Request::GetContentLength() const { return mContentLength; }

std::string const& Request::GetHost() const { return mHost; }

std::string const& Request::GetContentType() const { return mContentType; }

std::string const& Request::GetMethod() const { return mMethod; }

std::string const& Request::GetUri() const { return mUri; }

int Request::GetVersionMajor() const { return mVersionMajor; }

int Request::GetVersionMinor() const { return mVersionMinor; }

std::multimap<std::string, std::string> const& Request::GetHeaders() const {
  return mHeaders;
}

std::string const& Request::GetContent() const { return mContent; }

void Request::PushBackMethod(char &c) { mMethod.push_back(c); }

void Request::PushBackUri(char &c) { mUri.push_back(c); }

void Request::PushBackContent(char &c) { mContent.push_back(c); }

void Request::InsertHeader(std::string key, std::string value) {
  mHeaders.insert(std::pair<std::string, std::string>(key, value));
}

void Request::SetChunked(bool chunked) { mChunked = chunked; }

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
