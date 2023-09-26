#include "Tokenizer.hpp"

std::vector<std::string> Tokenizer::makeTokens(const std::string& path)
{
	std::stringstream confBuffer;
	std::vector<std::string> Tokens;

  openConfFile(path, confBuffer);
  removeComment(confBuffer);
  addBlank(confBuffer);
  tokenize(confBuffer, Tokens);
  return (Tokens);
}

void Tokenizer::openConfFile(const std::string &path, std::stringstream& outConfBuffer) {
  std::ifstream confFile(path);
  if (confFile.is_open() == false) {
    tokenError("Error: Could not open confFile ");
  }

  outConfBuffer << confFile.rdbuf();
  confFile.close();
  if (outConfBuffer.good() == false) {
    tokenError("Error: confFile stream ");
  }
}

void Tokenizer::removeComment(std::stringstream& outConfBuffer) {
  std::stringstream cleanBuffer;
  std::string line;
  int commentPos;

  while (std::getline(outConfBuffer, line)) {
    commentPos = line.find('#');
    if (commentPos != std::string::npos)
      line.erase(commentPos);
    cleanBuffer << line << std::endl;
  }
  outConfBuffer.str(cleanBuffer.str());
  outConfBuffer.clear();
}

void Tokenizer::addBlank(std::stringstream& outConfBuffer) {
  std::string content;
  content = outConfBuffer.str();
  for (int index = 0; index < content.size(); index++) {
    if (content[index] == '{' || content[index] == '}' ||
        content[index] == ';') {
      content.insert(index, " ");
      index += 2;
      content.insert(index, " ");
    }
  }
  outConfBuffer.str("");
  outConfBuffer << content;
}

void Tokenizer::tokenize(std::stringstream& outConfBuffer, std::vector<std::string>& outTokens) {
  std::string token;

  while (true) {
    outConfBuffer >> token;
    if (outConfBuffer.eof() == true) {
      break;
    }
    outTokens.push_back(token);
    token.clear();
  }
}

void Tokenizer::tokenError(const std::string &msg)
{
  std::cerr << "Token Error" << std::endl;
  std::cerr << msg << std::endl;
  exit(1);
}
