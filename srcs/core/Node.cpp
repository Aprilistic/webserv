#include "Config.hpp"

Node::Node(std::vector<std::string> &configTokens) : mLevel(NONE_LEVEL) {
  std::vector<std::string>::iterator token = configTokens.begin();
  CreateTree(HTTP, this, configTokens, token);
}

int Node::getTokenType(std::string token) {
  if (token == "{") {
    return (OPEN_BRACKET);
  } else if (token == "}") {
    return (CLOSE_BRACKET);
  } else if (token == ";") {
    return (SEMICOLON);
  } else {
    if (std::find(HTTPDirectives, HTTPDirectives + 5, token) !=
        HTTPDirectives + 5) {
      return (DIRECTIVE);
    }
    if (std::find(ServerDirectives, ServerDirectives + 2, token) !=
        ServerDirectives + 2) {
      return (DIRECTIVE);
    }
    if (std::find(LocationDirectives, LocationDirectives + 1, token) !=
        LocationDirectives + 1) {
      return (DIRECTIVE);
    }
  }
  return (UNDEFINED);
}

int Node::getAllowedOptions(std::string directive) {
  int ret;
  /* Option check return*/

  return ret;
}

void Node::CreateTree(int curLevel, Node *parent,
                      std::vector<std::string> &configTokens,
                      std::vector<std::string>::iterator &token) {
  int tokenType;
  int allowedOptions = 0;

  for (; token != configTokens.end(); token++) {
    /* Recursively create nodes and continue check syntax */
    if (getTokenType(*token))
      CreateTree(curLevel + 1, parent, configTokens, token);
    else
      LineParse(curLevel, parent, configTokens, token);
  }
}
