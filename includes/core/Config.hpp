#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Node.hpp"

class Config {
  /* methods */
public:
  Config(const std::string &path);
  ~Config(void);
  void SetConfig(const std::string &path);

private:
  void openConfFile(const std::string &path);
  void printConfFile(void);
  void printTree(void);
  void removeComment(void);
  void addBlank(void);
  void tokenize(void);

  void createTree(void);
  void deleteTree(void);
  /* variables */
private:
  std::stringstream mConfBuffer;
  Node *mConfigTree;
  std::vector<std::string> mTokens;
};

#endif