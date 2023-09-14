#include "Config.hpp"

Config::Config(const std::string &path,
               std::map<int, std::vector<Server *> > mServersMap) {
  SetConfig(path);
//     printTree();
  checkSum();

  // tokenize
  // createTree

  // validate or checksum or ...

  // CreatServer(mServersMap);
}

void Config::CreateServer(std::map<int, std::vector<Server *> > mServersMap) {
  int serverCount = 4; // mConfigTree -> n

  for (int i = 0; i < serverCount; i++) {
    mServersMap[i].push_back(new Server(mConfigTree));
  }
}

Config::~Config(void) {
  deleteTree();
  delete mConfigTree;
}

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

void Config::deleteTree(void) { mConfigTree->deleteTree(); }

void Config::printTree(void) { mConfigTree->PrintTree(0); }

void Config::checkSum(void) { 
  mConfigTree->checkSum(0);
}
