#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include "Node.hpp"
#include "Common.hpp"


// class Node;
class Config {
public:
  static void makeConfigTree(const std::string &path);
//   static std::vector<ServerConfig *> makeServerConfigList(WebServer *webServer, Node *configTree);
  static std::map<int, ServerConfig *> makeServerConfigList(WebServer *webServer,
                                                         Node *configTree);

private:
  Config();
  static void checkSum(Node *configTree);

  static void checkErrorPage(std::vector<std::string> &value);
  static void checkClientMaxBodySize(std::vector<std::string> &value);
  static void checkIndex(std::vector<std::string> &value);
  static void checkAutoIndex(std::vector<std::string> &value);
  static void checkListen(std::vector<std::string> &value);
  static void checkReturn(std::vector<std::string> &value);
  static void checkServerName(std::vector<std::string> &value);
  static void checkAlias(std::vector<std::string> &value);
  static void checkLimitExcept(std::vector<std::string> &value);

  static void configError(const std::string &msg);
};

#endif

