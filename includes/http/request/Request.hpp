#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Core.hpp"

class Request {
public:
  Request();
  ~Request();

  // struct HeaderItem {
  //   std::string name;
  //   std::string value;
  // };

  std::string GetMethod() const;
  std::string GetUri() const;
  int GetVersionMajor() const;
  int GetVersionMinor() const;
  // std::vector<HeaderItem> GetHeaders() const;
  std::multimap<std::string, std::string> GetHeaders() const;
  // std::vector<char> GetContent() const;
  std::string GetHost() const;
  int GetContentLength() const;
  std::string GetContentType() const;
  bool GetKeepAlive() const;

  void SetMethod(const std::string &method);
  void SetUri(const std::string &uri);
  void SetVersionMajor(int versionMajor);
  void SetVersionMinor(int versionMinor);
  // void SetHeaders(const std::vector<HeaderItem> &headers);
  void SetHeaders(const std::multimap<std::string, std::string> &headers);
  // void SetContent(const std::vector<char> &content);
  void SetHost(const std::string &host);
  void SetContentLength(int contentLength);
  void SetContentType(const std::string &contentType);
  void SetKeepAlive(bool keepAlive);

  std::string Inspect() const;

public:
  std::string mMethod;
  std::string mUri;
  int mVersionMajor;
  int mVersionMinor;
  // std::vector<HeaderItem> mHeaders;
  std::multimap<std::string, std::string> mHeaders;
  std::vector<char> mContent;

  std::string mHost;
  int mContentLength;
  std::string mContentType;
  bool mKeepAlive;
};

#endif