#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "Core.hpp"

class Token {
public:
  static std::vector<std::string> makeTokens(const std::string& path);

private:
  Token();
  static void openConfFile(const std::string &path, std::stringstream& outConfBuffer);
  static void removeComment(std::stringstream& outConfBuffer);
  static void addBlank(std::stringstream& outConfBuffer);
  static void tokenize(std::stringstream& outConfBuffer, std::vector<std::string>& outTokens);
  static void tokenError(const std::string &msg);

public:
private:
};

#endif