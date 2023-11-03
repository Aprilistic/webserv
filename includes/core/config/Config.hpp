#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Common.hpp"
#include "ConfigMap.hpp"
#include "Node.hpp"
#include "Server.hpp"

// class Node;
class Config {
public:
  static void MakeConfigTree(const std::string &path);
  static std::map<int, Server *> MakeServerList();
  static void MakeConfigMap();

private:
  Config();
  static void checkSum(Node *configTree);
  static void checkErrorPage(std::vector<std::string> &value);
  static void checkConnectionMaxBodySize(std::vector<std::string> &value);
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
