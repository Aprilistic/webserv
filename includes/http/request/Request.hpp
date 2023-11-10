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
  bool GetChunked() const;

  void PushBackMethod(char &input);
  void PushBackUri(char &input);
  void InsertHeader(std::string key, std::string value);

  void SetChunked(bool chunked);
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

private:
  std::string mMethod;
  bool mChunked;
  std::string mUri;
  int mVersionMajor;
  int mVersionMinor;
  std::multimap<std::string, std::string> mHeaders;
  std::string mHost;

public:
  std::string mContent;

  int mContentLength;
  std::string mContentType;
  bool mKeepAlive;
};

#endif