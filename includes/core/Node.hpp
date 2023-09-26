#ifndef NODE_HPP
#define NODE_HPP

#include "Core.hpp"

class Node {
public:
  Node(std::vector<std::string> &configTokens,
       std::vector<std::string>::iterator &tokenLocation, Node *parent, int level);
  ~Node(void);
  void PrintTree(int level);
	void CheckSum(int level);
private:
  Node();
  int getTokenInfo(std::string token);
  void addDirective(std::vector<std::string> &configTokens,
       std::vector<std::string>::iterator &tokenLocation);
  void nodeError(const std::string &msg);

  void checkErrorPage(std::vector<std::string> &value);
  void checkClientMaxBodySize(std::vector<std::string> &value);
  void checkIndex(std::vector<std::string> &value);
  void checkAutoIndex(std::vector<std::string> &value);
  void checkListen(std::vector<std::string> &value);
  void checkReturn(std::vector<std::string> &value);
  void checkServerName(std::vector<std::string> &value);
  void checkAlias(std::vector<std::string> &value);
  void checkLimitExcept(std::vector<std::string> &value);
  void deleteTree(void);

public:
  std::map<std::string, std::vector<std::string> > mDirectives;
  std::vector<Node *> mChildren;
  int mLevel;

private:
  Node *mParent;
};



#endif