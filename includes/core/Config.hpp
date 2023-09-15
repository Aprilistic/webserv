#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Node.hpp"
#include "Token.hpp"

// class Node;
class Config {
public:
  Config(const std::string &path);
  ~Config(void);
  void SetConfig(const std::string &path);
  Node *GetConfigTree(void) const;
private:
  Config();
  void printConfFile(void);
  void printTree(void);
  void checkSum(void);

  void createTree(void);
  void deleteTree(void);
  void configError(const std::string &msg);
public:
private:
  Node *mConfigTree;
  std::vector<std::string> mTokens;
};

#endif
