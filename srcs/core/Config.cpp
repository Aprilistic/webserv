#include "Config.hpp"

Config::Config(const std::string &path) { SetConfig(path); }

Config::~Config(void) { deleteTree(); }

void Config::SetConfig(const std::string &path) {
  openConfFile(path);
  removeComment();
  addBlank();
  tokenize();
  printConfFile();
}

void Config::openConfFile(const std::string &path) {
  std::ifstream confFile(path);
  if (confFile.is_open() == false) {
    Error("Error: Could not open confFile ");
  }

  mConfBuffer << confFile.rdbuf();
  confFile.close();
  if (mConfBuffer.good() == false) {
    Error("Error: confFile stream ");
  }
}

void Config::removeComment(void) {
  std::stringstream cleanBuffer;
  std::string line;
  int commentPos;

  while (std::getline(mConfBuffer, line)) {
    commentPos = line.find('#');
    if (commentPos != std::string::npos)
      line.erase(commentPos);
    cleanBuffer << line << std::endl;
  }
  mConfBuffer.str(cleanBuffer.str());
  mConfBuffer.clear();
}

void Config::addBlank(void) {
  std::string content;
  content = mConfBuffer.str();
  for (int index = 0; index < content.size(); index++) {
    if (content[index] == '{' || content[index] == '}' ||
        content[index] == ';') {
      content.insert(index, " ");
      index += 2;
      content.insert(index, " ");
    }
  }
  mConfBuffer.str("");
  mConfBuffer << content;
}

void Config::tokenize(void) {
  std::string token;

  while (true) {
    mConfBuffer >> token;
    if (mConfBuffer.eof() == true) {
      break;
    }
    mTokens.push_back(token);
    token.clear();
  }
}

void Config::printConfFile(void) {
  for (std::vector<std::string>::iterator it = mTokens.begin();
       it != mTokens.end(); ++it) {
    std::cout << *it << std::endl;
  }
}

void Config::createTree(void) {
  std::vector<std::string>::iterator token = mTokens.begin();
  mConfigTree = new Node(mTokens, token, NULL, 0);
}

void Config::deleteTree(void) { delete mConfigTree; }