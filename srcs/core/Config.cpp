#include "Config.hpp"

Config::Config(const std::string &path) { SetConfig(path); }

Config::~Config(void) { deleteTree(); }

void Config::SetConfig(const std::string &path) {
  Token token(path);
  mTokens = token.mTokens;
  createTree();
  // printTree();
}

void Config::createTree(void) {
  std::vector<std::string>::iterator token = mTokens.begin();
  mConfigTree = new Node(mTokens, token, NULL, 1);
}

void Config::printTree(void) { mConfigTree->PrintTree(0); }

void Config::deleteTree(void) { delete mConfigTree; }