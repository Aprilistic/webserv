#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Core.hpp"

class Request {
public:
  Request();
  ~Request();

  std::string GetMethod() const;
  std::string GetUri() const;
  int GetVersionMajor() const;
  int GetVersionMinor() const;
  std::multimap<std::string, std::string> GetHeaders() const;
  std::string GetContent() const;
  std::string GetHost() const;
  int GetContentLength() const;
  std::string GetContentType() const;
  bool GetKeepAlive() const;

  void SetMethod(const std::string &method);
  void SetUri(const std::string &uri);
  void SetVersionMajor(int versionMajor);
  void SetVersionMinor(int versionMinor);
  void SetHeaders(const std::multimap<std::string, std::string> &headers);
  void SetContent(const std::string &content);
  void SetHost(const std::string &host);
  void SetContentLength(int contentLength);
  void SetContentType(const std::string &contentType);
  void SetKeepAlive(bool keepAlive);

  std::string Inspect() const;

public:
  bool mChunked;
  std::string mMethod;
  std::string mUri;
  int mVersionMajor;
  int mVersionMinor;
  std::multimap<std::string, std::string> mHeaders;
  std::string mContent;

  std::string mHost;
  int mContentLength;
  std::string mContentType;
  bool mKeepAlive;
};

#endif