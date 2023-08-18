#include "Config.hpp"

// Node::Node(std::vector<std::string> &configTokens) : mLevel(NONE) {
//   CreateTree(NONE, this, configTokens, configTokens.begin());
// }

int Node::getTokenType(std::string token) {
  if (token == "{") {
    return (OPEN_BRACKET);
  } else if (token == "}") {
    return (CLOSE_BRACKET);
  } else if (token == ";") {
    return (SEMICOLON);
  } else {
    if (std::find(HTTPDirectives.begin(), HTTPDirectives.end(), token) !=
        HTTPDirectives.end()) {
      return (DIRECTIVE);
    }
    if (std::find(ServerDirectives.begin(), ServerDirectives.end(), token) !=
        ServerDirectives.end()) {
      return (DIRECTIVE);
    }
    if (std::find(LocationDirectives.begin(), LocationDirectives.end(),
                  token) != LocationDirectives.end()) {
      return (DIRECTIVE);
    }
  }
  return (UNDEFINED);
}

long long Node::getAllowedOptions(std::string directive) {
  long long ret;
  /* Option check return*/

  return ret;
}

void Node::CreateTree(int curLevel, Node *parent,
                      std::vector<std::string> &configTokens,
                      std::vector<std::string>::iterator &token) {

  int tokenType;
  long long allowedOptions = 0;

  for (; token != configTokens.end(); token++) {
    /* Recursively create nodes and continue check syntax */
  }
}