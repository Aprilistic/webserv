#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"

/* Level Bit Flags */
#define NONE_LEVEL (0)
#define HTTP (1 << 0)
#define SERVER (1 << 1)
#define LOCATION (1 << 2)

/* Value Count Bit Flags */
#define NONE_VALUE (1 << 3)
#define SINGLE (1 << 4)
#define MULTIPLE (1 << 5)

#define NEED_BRAKETS (1 << 6)
#define NEED_SEMICOLON (1 << 7)
#define NEED_BRANCH (1 << 8)

/* Token Type Flags */
#define DIRECTIVE (1 << 0)
#define OPEN_BRACKET (1 << 1)
#define CLOSE_BRACKET (1 << 2)
#define SEMICOLON (1 << 3)
#define UNDEFINED (1 << 4)

const std::string HTTPDirectives[] = {"client_max_body_size", "error_page",
                                      "autoindex", "index", "alias"};
const std::string ServerDirectives[] = {"server_name", "listen", "return"};
const std::string LocationDirectives[] = {"limit_except"};

class Node {
public:
  Node(std::vector<std::string> &configTokens);
  void CreateTree(int curLevel, Node *parent,
                  std::vector<std::string> &configTokens,
                  std::vector<std::string>::iterator &token);

private:
  int getAllowedOptions(std::string directive);
  int getTokenType(std::string directive);

public:
private:
  std::map<std::string, std::vector<std::string>> mDirectives;
  std::vector<Node> mChildren;
  std::map<std::string, Node *> mHashMap;
  int mLevel;
};

class Config {
  /* methods */
public:
  Config(const std::string &path);
  void SetConfig(const std::string &path);

private:
  void openConfFile(const std::string &path);
  void parseConfFile(void);
  void removeComment(void);
  void addBlank(void);
  void tokenize(void);

  /* variables */
private:
  std::stringstream mConfBuffer;
  //   std::map<std::string, ServerBlock> mServers;
  Node *mConfigTree;
  std::vector<std::string> mTokens;
};

#endif