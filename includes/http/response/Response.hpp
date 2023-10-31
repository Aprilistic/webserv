#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Core.hpp"

class Response {
public:
  Response();
  ~Response();

  // struct HeaderItem {
  //   std::string name;
  //   std::string value;
  // };
  int GetVersionMajor() const;
  int GetVersionMinor() const;
  unsigned int GetStatusCode() const;
  std::string GetStatus() const;
  // std::vector<HeaderItem> GetHeaders() const;
  std::multimap<std::string, std::string> GetHeaders() const;
  // std::vector<char> GetContent() const;
  std::string GetBody() const;
  bool GetKeepAlive() const;

  void SetVersionMajor(int versionMajor);
  void SetVersionMinor(int versionMinor);
  void SetStatusCode(unsigned int statusCode);
  void SetStatus(const std::string &status);
  // void SetHeaders(const std::vector<HeaderItem> &headers);
  void SetHeaders(const std::multimap<std::string, std::string> &headers);
  // void SetContent(const std::vector<char> &content);
  void SetBody(const std::string &body);
  void SetKeepAlive(bool keepAlive);

  std::string Inspect() const;

public:
  int mVersionMajor;
  int mVersionMinor;
  unsigned int mStatusCode;
  std::string mStatus;
  // std::vector<HeaderItem> mHeaders;
  std::multimap<std::string, std::string> mHeaders;
  // std::vector<char> mContent;
  std::string mBody;
  bool mKeepAlive;

  std::string mFilename;
};

#endif
