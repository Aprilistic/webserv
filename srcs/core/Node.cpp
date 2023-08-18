#include "Node.hpp"

/* Level Bit Flags */
#define NONE_LEVEL (0)
#define HTTP_LEVEL (1 << 0)
#define SERVER_LEVEL (1 << 1)
#define LOCATION_LEVEL (1 << 2)
#define CREATE_NODE (1 << 3)
#define DIRECTIVE (1 << 4)
#define SINGLE_VALUE (1 << 5)
#define MULTI_VALUE (1 << 6)
#define VALUE (1 << 7)
#define ERROR (1 << 8)

/* context */
#define HTTP (HTTP_LEVEL | CREATE_NODE)
#define SERVER (SERVER_LEVEL | CREATE_NODE)
#define LOCATION (LOCATION_LEVEL | CREATE_NODE)
/* http server location */
#define ERROR_PAGE                                                             \
  (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
#define CLIENT_MAX_BODY_SIZE                                                   \
  (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | SINGLE_VALUE)
#define AUTO_INDEX                                                             \
  (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | SINGLE_VALUE)
#define INDEX                                                                  \
  (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
/* server */
#define LISTEN (SERVER_LEVEL | DIRECTIVE | MULTI_VALUE)
#define SERVER_NAME (SERVER_LEVEL | DIRECTIVE | SINGLE_VALUE)
/* server location */
#define RETURN (SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
/* location */
#define ALIAS (LOCATION_LEVEL | DIRECTIVE | SINGLE_VALUE)
#define LIMIT_EXCEPT (LOCATION_LEVEL | DIRECTIVE | MULTI_VALUE)
/* { } ; */
#define OPEN_BRACKET (NONE_LEVEL | HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL)
#define CLOSE_BRACKET (NONE_LEVEL | HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL)
#define SEMICOLON (NONE_LEVEL | HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL)

Node::Node(std::vector<std::string> &configTokens,
           std::vector<std::string>::iterator &token, Node *parent, int level)
    : mParent(parent) {
  int tokenInfo;

  for (; token != configTokens.end(); token++) {
    tokenInfo = getTokenInfo(*token);
    token++;

    if ((tokenInfo & level) == 0){
      std::cerr << "Error: Invalid token level" << std::endl;
      exit(1);
    }

    if (tokenInfo & CREATE_NODE) {
      Node *newNode = new Node(configTokens, token, this, level + 1);
      mChildren.push_back(newNode);
    } else if (tokenInfo & DIRECTIVE) {
      addDirective(configTokens, token);
    } else if (tokenInfo & OPEN_BRACKET) {
    } else if (tokenInfo & CLOSE_BRACKET) {
      return;
    }
  }
}

Node::~Node(void) { deleteTree(); }

int Node::getTokenInfo(std::string token) {
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
  return (VALUE);
}

void Node::addDirective(std::vector<std::string> &configTokens,
       std::vector<std::string>::iterator &token){

}

void Node::deleteTree(void) {
  if (mChildren.size() == 0)
    return;
  for (std::vector<Node *>::iterator it = mChildren.begin();
       it != mChildren.end(); ++it) {
    (*it)->deleteTree();
    delete (*it);
  }
}