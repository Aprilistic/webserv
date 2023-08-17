#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"

class ServerBlock;
class LocationBlock;
class Config {
  /* methods */
public:
  Config(const std::string &path);
  int SetConfig(const std::string &path);

private:
  void checkSyntax(void);
  void openConfFile(const std::string &path);
  void parseConfFile(void);
  void tokenizer(void);
  /* variables */
private:
  std::stringstream mConfBuffer;
  std::map<std::string, ServerBlock> mServers;
};
class ServerBlock {
public:
  ServerBlock(void);
  ~ServerBlock(void);

private:
  LocationBlock *mRoot;
  std::map<std::string, LocationBlock *> mLocation; /* URI 경로, 노드 주소 */
};

class LocationBlock {
public:
private:
  std::string mFragment; /* URI */
  std::map<std::string, std::vector<std::string>> mDirectives;
  LocationBlock *mParent; /* NULL이면 rootNode */
  std::map<std::string, LocationBlock *> mChildren;
};

#endif