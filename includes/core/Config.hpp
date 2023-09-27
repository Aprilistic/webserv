#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Tokenizer.hpp"
#include "ServerConfig.hpp"
#include "Node.hpp"

// class Node;
class Config {
public:
  static Node* makeConfigTree(const std::string &path);
//   static std::vector<ServerConfig *> makeServerConfigList(WebServer *webServer, Node *configTree);
  static std::map<int, ServerConfig *> makeServerConfigList(WebServer *webServer,
                                                         Node *configTree);
  static void printTree(void);

private:
  Config();
  static void checkSum(Node *configTree);

};

#endif

