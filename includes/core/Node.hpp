#ifndef NODE_HPP
#define NODE_HPP

#include "Core.hpp"
#include "Config.hpp"

class Node {
public:
  Node(std::vector<std::string> &configTokens,
       std::vector<std::string>::iterator &token, Node *parent, int level);
  ~Node(void);
  void PrintTree(int level);
	void checkSum(int level);
  void deleteTree(void);
private:
  int getTokenInfo(std::string token);
  void addDirective(std::vector<std::string> &configTokens,
       std::vector<std::string>::iterator &token);
  void nodeError(const std::string &msg);

  void CheckErrorPage(std::vector<std::string> &value);
  void CheckClientMaxBodySize(std::vector<std::string> &value);
  void CheckIndex(std::vector<std::string> &value);
  void CheckAutoIndex(std::vector<std::string> &value);
  void CheckListen(std::vector<std::string> &value);
  void CheckReturn(std::vector<std::string> &value);
  void CheckServerName(std::vector<std::string> &value);
  void CheckAlias(std::vector<std::string> &value);
  void CheckLimitExcept(std::vector<std::string> &value);

public:
  std::map<std::string, std::vector<std::string> > mDirectives;
  std::vector<Node *> mChildren;
private:
  Node *mParent;
};



#endif