#include "Config.hpp"

/* Level Bit Flags */
#define NONE_LEVEL (0)
#define HTTP_LEVEL (1 << 0)
#define SERVER_LEVEL (1 << 1)
#define LOCATION_LEVEL (1 << 2)
#define CREATE_NODE (1 << 3)
#define DIRECTIVE (1 << 4)
#define SINGLE_VALUE (1 << 5)
#define MULTI_VALUE (1 << 6)
#define ERROR (1 << 7)

/* context */
#define HTTP (HTTP_LEVEL | CREATE_NODE)
#define SERVER (SERVER_LEVEL | CREATE_NODE)
#define LOCATION (LOCATION_LEVEL | CREATE_NODE)
/* http server location */
#define ERROR_PAGE (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
#define CLIENT_MAX_BODY_SIZE (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | SINGLE_VALUE)
#define AUTO_INDEX (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | SINGLE_VALUE)
#define INDEX (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
/* server */
#define LISTEN (SERVER_LEVEL | DIRECTIVE | MULTI_VALUE)
#define SERVER_NAME (SERVER_LEVEL | DIRECTIVE | SINGLE_VALUE)
/* server location */
#define RETURN (SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
/* location */
#define ALIAS (LOCATION_LEVEL | DIRECTIVE | SINGLE_VALUE)
#define LIMIT_EXCEPT (LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
/* { } ;*/
#define OPEN_BRACKET (NONE_LEVEL | HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL)
#define CLOSE_BRACKET (NONE_LEVEL | HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL)
#define SEMICOLON (NONE_LEVEL | HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL)

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
  } else if (token == "http") {
    return (HTTP);
  } else if (token == "server") {
    return (SERVER);
  } else if (token == "location") {
    return (LOCATION);
  } else if (token == "error_page") {
    return (ERROR_PAGE);
  } else if (token == "client_max_body_size") {
    return (CLIENT_MAX_BODY_SIZE);
  } else if (token == "autoindex") {
    return (AUTO_INDEX);
  } else if (token == "index") {
    return (INDEX);
  } else if (token == "listen") {
    return (LISTEN);
  } else if (token == "server_name") {
    return (SERVER_NAME);
  } else if (token == "return") {
    return (RETURN);
  } else if (token == "alias") {
    return (ALIAS);
  } else if (token == "limit_except") {
    return (LIMIT_EXCEPT);
  } 

  return (ERROR);
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
