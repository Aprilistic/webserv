#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "Core.hpp"

class Token {
public:
  Token(const std::string &path);

private:
  void openConfFile(const std::string &path);
  void removeComment(void);
  void addBlank(void);
  void tokenize(void);

public:
  std::vector<std::string> mTokens;
private:
  std::stringstream mConfBuffer;
};

#endif