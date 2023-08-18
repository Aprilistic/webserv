#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"


class Node {
public:
  Node(std::vector<std::string> &configTokens);
  void CreateTree(int curLevel, Node *parent,
                  std::vector<std::string> &configTokens,
                  std::vector<std::string>::iterator &token);

private:
  int getTokenType(std::string token);

public:
private:
  std::map<std::string, std::vector<std::string>> mDirectives;
  std::vector<Node> mChildren;
  std::map<std::string, Node *> mHashMap;
  int mLevel;
};

class Config {
  /* methods */
public:
  Config(const std::string &path);
  void SetConfig(const std::string &path);

private:
  void openConfFile(const std::string &path);
  void parseConfFile(void);
  void removeComment(void);
  void addBlank(void);
  void tokenize(void);

  /* variables */
private:
  std::stringstream mConfBuffer;
  //   std::map<std::string, ServerBlock> mServers;
  Node *mConfigTree;
  std::vector<std::string> mTokens;
};

#endif