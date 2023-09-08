#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Node.hpp"
#include "Token.hpp"
#include "Server.hpp"

class Config {
  /* methods */
public:
  Config(const std::string &path, std::map<int, std::vector<Server *>> mServersMap);
  ~Config(void);
  void SetConfig(const std::string &path);
  void CreateServer(std::map<int, std::vector<Server *>> mServersMap);
private:
  void printConfFile(void);
  void printTree(void);

  void createTree(void);
  void deleteTree(void);
  /* variables */
private:
  Node *mConfigTree;
  std::vector<std::string> mTokens;
};

#endif
