#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "Core.hpp"

class Tokenizer {
public:
  static std::vector<std::string> makeTokens(const std::string &path);

private:
  Tokenizer();
  static void openConfFile(const std::string &path,
                           std::stringstream &outConfBuffer);
  static void removeComment(std::stringstream &outConfBuffer);
  static void addBlank(std::stringstream &outConfBuffer);
  static void tokenize(std::stringstream &outConfBuffer,
                       std::vector<std::string> &outTokens);
};

#endif