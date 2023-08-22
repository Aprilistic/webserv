#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"
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
  void removeComment(void);
  void addBlank(void);
  void tokenize(void);

  void createTree(void);
  void deleteTree(void);
  /* variables */
private:
  std::stringstream mConfBuffer;
  //   std::map<std::string, ServerBlock> mServers;
  Node *mConfigTree;
  std::vector<std::string> mTokens;
};

#endif