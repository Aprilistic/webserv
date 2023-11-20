#include "Node.hpp"
/* Level Bit Flags */
#define NONE_LEVEL (1 << 0)
#define HTTP_LEVEL (1 << 1)
#define SERVER_LEVEL (1 << 2)
#define LOCATION_LEVEL (1 << 3)
#define CREATE_NODE (1 << 4)
#define DIRECTIVE (1 << 5)
#define OPEN_BRACKET (1 << 6)
#define CLOSE_BRACKET (1 << 7)
#define SEMICOLON (1 << 8)
#define VALUE (1 << 9)

/* context */
#define HTTP (NONE_LEVEL | CREATE_NODE)
#define SERVER (HTTP_LEVEL | CREATE_NODE)
#define LOCATION (SERVER_LEVEL | LOCATION_LEVEL | CREATE_NODE)
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
#define CGI_PASS (LOCATION_LEVEL | DIRECTIVE)
/* { } ; */

Node::Node(std::vector<std::string> &configTokens,
           std::vector<std::string>::iterator &tokenLocation, Node *parent,
           int level)
    : mParent(parent), mLevel(level) {
  int tokenInfo;

  for (; tokenLocation != configTokens.end(); tokenLocation++) {
    tokenInfo = getTokenInfo(*tokenLocation);

    if ((tokenInfo & level) == 0 && tokenInfo != CLOSE_BRACKET) {
      nodeError("Error: Invalid token level");
    }

    if (tokenInfo & CREATE_NODE) {
      bool bLocationFlag = false;
      std::string location;
      if (tokenInfo == LOCATION) { /* location */
        tokenInfo = getTokenInfo(*(++tokenLocation));
        bLocationFlag = true;
        location = *tokenLocation;
      }

      tokenInfo = getTokenInfo(*(++tokenLocation));
      if ((tokenInfo & OPEN_BRACKET) == 0) {
        nodeError("Error: Bracket not opened");
      }

      Node *newNode = new Node(configTokens, ++tokenLocation, this,
                               (level & LOCATION_LEVEL) ? level : level << 1);
      if (bLocationFlag) {
        newNode->mDirectives["location"].push_back(location);
      }
      mChildren.push_back(newNode);
    } else if (tokenInfo & OPEN_BRACKET) {
      nodeError("Error: Wrong bracket");
    } else if (tokenInfo & CLOSE_BRACKET) {
      if (level == NONE_LEVEL && (tokenLocation != configTokens.end())) {
        nodeError("Error: Wrong bracket");
      }
      return;
    } else {
      addDirective(configTokens, tokenLocation);
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
  } else if (token == "cgi_pass") {
    return (CGI_PASS);
  }
  return (VALUE);
}

void Node::addDirective(std::vector<std::string> &configTokens,
                        std::vector<std::string>::iterator &tokenLocation) {
  std::string key = *tokenLocation;

  if (mDirectives.find(key) != mDirectives.end()) {
    nodeError("Error: Directive already exists");
  }
  std::vector<std::string> values;
  tokenLocation++;
  for (; tokenLocation != configTokens.end(); tokenLocation++) {
    int tokenInfo = getTokenInfo(*tokenLocation);
    if (tokenInfo & SEMICOLON) {
      break;
    }
    values.push_back(*tokenLocation);
  }
  if (tokenLocation == configTokens.end()) {
    nodeError("Error: Syntax Error");
  }
  mDirectives[key] = values;
}

void Node::PrintTree(int level) {
  for (int i = 0; i < level; i++) {
    printf("  ");
  }
  for (std::map<std::string, std::vector<std::string> >::iterator it =
           mDirectives.begin();
       it != mDirectives.end(); ++it) {
    for (int i = 0; i < level; i++) {
      printf("  ");
    }
    printf("key = %s\n", it->first.c_str());
    for (int i = 0; i < level; i++) {
      printf("  ");
    }
    printf("values = ");
    for (std::vector<std::string>::iterator it2 = it->second.begin();
         it2 != it->second.end(); ++it2) {
      printf("%s ", it2->c_str());
    }
    printf("\n");
  }
  for (std::vector<Node *>::iterator it = mChildren.begin();
       it != mChildren.end(); ++it) {
    (*it)->PrintTree(level + 1);
  }
}

void Node::deleteTree(void) {
  for (std::vector<Node *>::iterator it = mChildren.begin();
       it != mChildren.end(); ++it) {
    (*it)->deleteTree();
    delete (*it);
  }
  mChildren.clear();
}

void Node::nodeError(const std::string &msg) {
  Node *currentNode = this;
  while (currentNode->mParent != NULL) {
    currentNode = currentNode->mParent;
  }
  currentNode->deleteTree();
  throw std::runtime_error(msg);
}

// if there is no value, return empty vector == segmentation fault
std::vector<std::string> Node::FindValue(Node *current, std::string key) {
  if (current == NULL) {
    return std::vector<std::string>();
  }

  if (current->mDirectives.count(key) == 0) {
    return FindValue(current->mParent, key);
  }
  return current->mDirectives[key];
}

std::vector<std::string> Node::FindTopValue(Node *current, std::string key,
                                            std::vector<std::string> value) {
  if (current == NULL) {
    if (value.size() == 0) {
      return std::vector<std::string>();
    } else {
      return value;
    }
  }

  if (current->mDirectives.count(key) != 0) {
    value = current->mDirectives[key];
  }
  return FindTopValue(current->mParent, key, value);
}
