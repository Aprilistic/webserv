#include "Config.hpp"

Config::Config(const std::string &path, Node* mConfigTree)
{
	SetConfig(path, mConfigTree);
		// tokenize
		// createTree
	//validate
		// 
}

Config::~Config(void)
{
	// deleteTree(mConfigTree);
}

void Config::SetConfig(const std::string &path, Node* mConfigTree) {
  Token token(path);
  mTokens = token.mTokens;
  createTree(mConfigTree);
  // printTree();
}

void Config::createTree(Node* mConfigTree) {
  std::vector<std::string>::iterator token = mTokens.begin();
  mConfigTree = new Node(mTokens, token, NULL, 1);
}

void Config::printTree(Node* mConfigTree) { mConfigTree->PrintTree(0); }

// void Config::deleteTree(void) { delete mConfigTree; }