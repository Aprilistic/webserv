#include "Node.hpp"

/* Level Bit Flags */
#define NONE_LEVEL (1)
#define HTTP_LEVEL (1 << 1)
#define SERVER_LEVEL (1 << 2)
#define LOCATION_LEVEL (1 << 3)
#define CREATE_NODE (1 << 4)
#define DIRECTIVE (1 << 5)
#define ERROR (1 << 6)

/* context */
#define HTTP (HTTP_LEVEL | CREATE_NODE)
#define SERVER (SERVER_LEVEL | CREATE_NODE)
#define LOCATION (LOCATION_LEVEL | CREATE_NODE)
/* http server location */
#define ERROR_PAGE (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE)
#define CLIENT_MAX_BODY_SIZE                                                   \
  (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE)
#define AUTO_INDEX (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE)
#define INDEX (HTTP_LEVEL | SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE)
/* server */
#define LISTEN (SERVER_LEVEL | DIRECTIVE)
#define SERVER_NAME (SERVER_LEVEL | DIRECTIVE)
/* server location */
#define RETURN (SERVER_LEVEL | LOCATION_LEVEL | DIRECTIVE)
/* location */
#define ALIAS (LOCATION_LEVEL | DIRECTIVE)
#define LIMIT_EXCEPT (LOCATION_LEVEL | DIRECTIVE)
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

    bCreateNode = tokenInfo & CREATE_NODE;

    if (tokenInfo == LOCATION) { /* location */
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
        addDirective(configTokens, token); /* 함수 종료 시, *token == ";" */
      }
    }
    /* 현재 토큰 위치 고려 */
    tokenInfo = getTokenInfo(*(++token));
    if (~(tokenInfo & CLOSE_BRACKET)) {
      Error("Error: Bracket not closed\n");
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
  return (ERROR);
}

void Node::addDirective(std::vector<std::string> &configTokens,
                        std::vector<std::string>::iterator &token) {
  std::string key = *token;

  if (mDirectives.find(key) != mDirectives.end()) {
    Error("Error: Directive already exists\n");
  }

  std::vector<std::string> values;
  token++;
  for (; token != configTokens.end(); token++) {
    int tokenInfo = getTokenInfo(*token);
    if (tokenInfo & (OPEN_BRACKET | CLOSE_BRACKET)) {
      Error("Error: Invalid number of arguments\n");
    }
    if (tokenInfo & SEMICOLON) {
      break;
    }
    values.push_back(*token);
  }
  mDirectives[key] = values;
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