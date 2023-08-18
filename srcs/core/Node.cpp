#include "Config.hpp"

// Node::Node(std::vector<std::string> &configTokens) : mLevel(NONE) {
//   CreateTree(NONE, this, configTokens, configTokens.begin());
// }

std::vector<std::string> initializeHTTPDirectives() {
    std::vector<std::string> temp;
    temp.push_back("client_max_body_size");
    temp.push_back("error_page");
    temp.push_back("autoindex");
    temp.push_back("index");
    temp.push_back("alias");
    temp.push_back("root");
    return temp;
}

std::vector<std::string> initializeServerDirectives() {
    std::vector<std::string> temp;
    temp.push_back("server_name");
    temp.push_back("listen");
    return temp;
}

std::vector<std::string> initializeLocationDirectives() {
    std::vector<std::string> temp;
    temp.push_back("location");
    return temp;
}

const std::vector<std::string> HTTPDirectives = initializeHTTPDirectives();
const std::vector<std::string> ServerDirectives = initializeServerDirectives();
const std::vector<std::string> LocationDirectives = initializeLocationDirectives();

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

