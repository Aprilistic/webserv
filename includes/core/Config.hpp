#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Node.hpp"
#include "Token.hpp"

class Config {
  /* methods */
public:
  Config(const std::string &path);
  ~Config(void);
  void SetConfig(const std::string &path);

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
