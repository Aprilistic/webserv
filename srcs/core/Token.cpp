#include "Config.hpp"

Token::Token(const std::string &path) {
  openConfFile(path);
  removeComment();
  addBlank();
  tokenize();
}

void Token::openConfFile(const std::string &path) {
  std::ifstream confFile(path);
  if (confFile.is_open() == false) {
    sendError("Error: Could not open confFile ");
  }

  mConfBuffer << confFile.rdbuf();
  confFile.close();
  if (mConfBuffer.good() == false) {
    sendError("Error: confFile stream ");
  }
}

void Token::removeComment(void) {
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

void Token::addBlank(void) {
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

void Token::tokenize(void) {
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