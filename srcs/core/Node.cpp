#include "Node.hpp"

/* Level Bit Flags */
#define NONE_LEVEL (1)
#define HTTP_LEVEL (1 << 1)
#define SERVER_LEVEL (1 << 2)
#define LOCATION_LEVEL (1 << 3)
#define CREATE_NODE (1 << 4)
#define DIRECTIVE (1 << 5)
#define SINGLE_VALUE (1 << 6)
#define MULTI_VALUE (1 << 7)
#define VALUE (1 << 8)
#define ERROR (1 << 9)

/* context */
#define HTTP (HTTP_LEVEL | CREATE_NODE)
#define SERVER (SERVER_LEVEL | CREATE_NODE)
#define LOCATION (LOCATION_LEVEL | SINGLE_VALUE | CREATE_NODE)
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
    bool bCreateNode = false;
    tokenInfo = getTokenInfo(*token);

    if ((tokenInfo & level) == 0) {
      Error("Error: Invalid token level");
    }

    if (tokenInfo & CREATE_NODE) {
      bCreateNode = true;
    }

    if (tokenInfo & (CREATE_NODE | SINGLE_VALUE)) { /* location */
      tokenInfo = getTokenInfo(*(++token));
      if (tokenInfo & (OPEN_BRACKET | CLOSE_BRACKET | SEMICOLON)) {
        Error("invalid number of arguments in \"location\" directive\n");
      }
    }

    if (bCreateNode == true) {
      tokenInfo = getTokenInfo(*(++token));
      if (~(tokenInfo & OPEN_BRACKET)) {
        Error("Error: Bracket not opened\n");
      }
    }

    if (bCreateNode) {
      Node *newNode = new Node(configTokens, ++token, this,
                               (level & LOCATION_LEVEL) ? level : level << 1);
      mChildren.push_back(newNode);
    } else {
      for (; token != configTokens.end(); token++) {
        /* addDirective 이후 토큰 위치 고려 */
        tokenInfo = getTokenInfo(*token);
        if (tokenInfo & CREATE_NODE) {
          break;
        }
        addDirective(configTokens, token);
      }
    }
  }
  /* 현재 토큰 위치 고려 */
  tokenInfo = getTokenInfo(*(++token));
  if (~(tokenInfo & CLOSE_BRACKET)) {
    Error("Error: Bracket not closed\n");
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
                        std::vector<std::string>::iterator &token) {
  std::string directive = *token;
  std::vector<std::string> values;

  std::map<std::string, std::vector<std::string>>::iterator it =
      mDirectives.find(directive);

  if (it != mDirectives.end()) {
  }
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