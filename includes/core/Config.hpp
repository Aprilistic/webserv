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

class CoreModule {
public:
protected:
  int mTimeOut; /* Round-Robin time slice */
  int mClientMaxBodySize;
  std::string mUploadStore;
};

class ServerContext : public CoreModule
{
public:
private:
  int mListen; // int mPort; std::string mHost;
  std::string mServerName;

  // location context
  std::string mRoot;
  std::vector<std::string> mIndex;
  std::vector<int> mErrorPageNum;
  std::string mErrorPage;
  std::map<std::string, std::map<std::string, std::string> > mLocations; /* <location_dir, <key, filename>> */

  /* ServerContext */
  /* LocationContext */
};

#endif
