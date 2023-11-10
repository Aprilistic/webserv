#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Core.hpp"

class Response {
public:
  Response();
  ~Response();

  int GetVersionMajor() const;
  int GetVersionMinor() const;
  unsigned int GetStatusCode() const;
  std::string GetStatus() const;
  std::multimap<std::string, std::string> GetHeaders() const;
  std::string GetBody() const;
  bool GetKeepAlive() const;

  void InsertHeader(const std::string &key, const std::string &value);

  void SetVersionMajor(int versionMajor);
  void SetVersionMinor(int versionMinor);
  void SetStatusCode(unsigned int statusCode);
  void SetStatus(const std::string &status);
  void SetBody(const std::string &body);
  void SetKeepAlive(bool keepAlive);

  std::string Inspect() const;

private:
  int mVersionMajor;
  int mVersionMinor;
  unsigned int mStatusCode;
  std::string mStatus;
  std::multimap<std::string, std::string> mHeaders;

public:
  std::string mBody;
  bool mKeepAlive;

  std::string mFilename;
};

#endif
