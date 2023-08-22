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
    tokenInfo = getTokenInfo(*token);

    if ((tokenInfo & level) == 0) {
      Error("Error: Invalid token level");
    }

    if (tokenInfo & CREATE_NODE) {
      token++;
      if (~(tokenInfo & OPEN_BRACKET)) {
        Error("Error: Invalid token level");
      }
      Node *newNode = new Node(configTokens, token, this, level + 1);
      if (tokenInfo & SINGLE_VALUE) {
        tokenInfo = getTokenInfo(*token);
        if (tokenInfo & (OPEN_BRACKET | CLOSE_BRACKET | SEMICOLON)) {
          Error("invalid number of arguments in \"location\" directive\n");
        } else {
          // location name
        }
        mChildren.push_back(newNode);
      } else if (tokenInfo & DIRECTIVE) {
        addDirective(configTokens, token);
      } else if (tokenInfo & OPEN_BRACKET) {

      } else if (tokenInfo & CLOSE_BRACKET) {
        return;
      }
    }
  }
}