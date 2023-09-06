#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Node.hpp"
#include "Token.hpp"

class Config {
  /* methods */
public:
  Config(const std::string &path, Node* mConfigTree);
  ~Config(void);
  void SetConfig(const std::string &path, Node* mConfigTree);

private:
  void printConfFile(void);
  void printTree(Node* mConfigTree);

  void createTree(Node* mConfigTree);
  void deleteTree(Node* mConfigTree);
  /* variables */
private:
  std::vector<std::string> mTokens;
};

#endif
