#ifndef NODE_HPP
#define NODE_HPP

#include "Core.hpp"

class Node {
public:
  Node(std::vector<std::string> &configTokens,
       std::vector<std::string>::iterator &token, Node *parent, int level);
  ~Node(void);
  void PrintTree(int level);

private:
  int getTokenInfo(std::string token);
  void addDirective(std::vector<std::string> &configTokens,
       std::vector<std::string>::iterator &token);
  void deleteTree(void);

public:
private:
  std::map<std::string, std::vector<std::string> > mDirectives;
  Node *mParent;
  std::vector<Node *> mChildren;
};

#endif