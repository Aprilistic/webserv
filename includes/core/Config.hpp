#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"

class Config {
  /* methods */
public:
  Config(const std::string &path);
  int SetConfig(const std::string &path);

protected:
private:
  void openConfFile(const std::string &path);
  void parseConfFile(void);

  /* variables */
protected:
private:
  std::stringstream mConfBuffer;
};

class ServerInfo {
public:
  void ParseConfig(std::string &fileName);
private:
  void setServer(int level, ServerInfo parentData);
  void setLocation(int level, LocationInfo parentData);

public:
private:
  std::map<int, std::string> mErrorPage;
  int mClientMaxBodySize;
  std::string mAlias;
  bool mbAutoIndex;
  std::string mIndex;
  std::map<int, std::string> mListen;
  std::string mServerName;
  std::map<int, std::string> mReturn;
  std::vector<std::string> mLimitExcept;
};
#endif
