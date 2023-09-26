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
#define LOCATION (SERVER_LEVEL | CREATE_NODE)
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

Node::Node(std::vector<std::string> &configTokens,
           std::vector<std::string>::iterator &tokenLocation, Node *parent, int level)
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
  delete (currentNode);
  throw std::runtime_error(msg);
}



void Node::CheckSum(int level) {
  for (std::map<std::string, std::vector<std::string> >::iterator it =
           mDirectives.begin();
       it != mDirectives.end(); ++it) {
    if (it->first == "error_page")
      checkErrorPage(it->second);
    else if (it->first == "client_max_body_size")
      checkClientMaxBodySize(it->second);
    else if (it->first == "index")
      checkIndex(it->second);
    else if (it->first == "autoindex")
      checkAutoIndex(it->second);
    else if (it->first == "listen")
      checkListen(it->second);
    else if (it->first == "server_name")
      checkServerName(it->second);
    else if (it->first == "return")
      checkReturn(it->second);
    else if (it->first == "alias")
      checkAlias(it->second);
    else if (it->first == "limit_except")
      checkLimitExcept(it->second);
  }
  for (std::vector<Node *>::iterator it = mChildren.begin();
       it != mChildren.end(); ++it) {
    (*it)->CheckSum(level + 1);
  }
}

void Node::checkErrorPage(std::vector<std::string> &value) {
  if (value.size() < 2) {
    nodeError("Error: Incorrect number of arguments. At least two arguments "
              "are required.");
  }
  for (std::vector<std::string>::iterator it = value.begin();
       it != value.end() - 1; it++) {
    for (std::string::iterator strIt = it->begin(); strIt != it->end();
         ++strIt) {
      if (isdigit(*strIt) == false) {
        nodeError("Error: Invalid character found in status code argument.");
      }
    }
    int statusCode = std::atoi(it->c_str());
    if (statusCode < 300 || statusCode > 599) {
      nodeError("Error: Status code out of valid range (300-599).");
    }
  }
  // 마지막값은 유효성 검사하지 않아도 됨
}

void Node::checkClientMaxBodySize(std::vector<std::string> &value) {
  if (value.size() != 1) {
    nodeError("Error: Incorrect number of arguments. Exactly one argument is "
              "required.");
  }
  int multiplier;
  char lastChar = value[0].back();
  lastChar = toupper(lastChar);

  switch (lastChar) {
  case 'K':
    multiplier = 1024; // KB
    break;
  case 'M':
    multiplier = (1024 * 1024); // MB
    break;
  case 'G':
    multiplier = 1024 * 1024 * 1024; // GB
    break;
  default:
    multiplier = 1;
    break;
  }

  std::string str;

  if (multiplier == 1) {
    str = value[0];
  } else {
    str = value[0].substr(0, value[0].size() - 1);
  }

  for (int i = 0; i < str.size(); i++) {
    if (isdigit(str[i] == false)) {
      nodeError("Error: Invalid character found in status code argument.");
    }
  }

  int num = atoi(value[0].c_str());

  if (num < 0) {
    nodeError("Error: Only positive numbers are possible.");
  }
  // mClientMaxBodySize = (num * multiplier);
}

void Node::checkIndex(std::vector<std::string> &value) {
  if (value.size() < 1) {
    nodeError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Node::checkAutoIndex(std::vector<std::string> &value) {

  if (value.size() != 1) {
    nodeError("Error: Incorrect number of arguments. Exactly one argument is "
              "required.");
  }
  std::string upperName = value[0];
  std::transform(upperName.begin(), upperName.end(), upperName.begin(),
                 ::toupper);
  if (upperName != "ON" && upperName != "OFF") {
    nodeError("The value is not ON or OFF!");
  }
}

void Node::checkListen(std::vector<std::string> &value) {
  if (value.size() < 1) {
    nodeError(
        "Error: No arguments provided. At least one argument is required.");
  }

  size_t pos = value[0].find(':');
  // only IP or only Port

  if (pos == std::string::npos) {
    if (value[0].find('.') != std::string::npos) {
      // Only IP
      std::istringstream iss(value[0]);
      int num;
      char dot;
      for (int i = 0; i < 3; i++) {
        iss >> num >> dot;
        if (num < 0 || 255 < num || dot != '.') {
          nodeError("Error: 0 ~ 255 & only .");
        }
      }
      iss >> num;
      if (num < 0 || 255 < num || iss.eof() == false) {
        nodeError("Error: 0 ~ 255 & only .");
      }
    } else {
      // Only Port
      for (int i = 0; i < value[0].size(); i++) {
        if (!isdigit(value[0][i])) {
          nodeError("Error: Invalid port number.");
        }
      }
      int port = atoi(value[0].c_str());
      if (port < 0 || 65535 < port) {
        nodeError("Error: 0 ~ 65535");
      }
    }
  } else {
    // IP:Port 127.0.0.1:8080
    std::string ip = value[0].substr(0, pos);       // ex 127.0.0.1
    std::string portStr = value[0].substr(pos + 1); // 8080
    int port = atoi(portStr.c_str());               // 8080

    // IP validate
    int num;
    char dot;
    std::istringstream iss(ip);

    for (int i = 0; i < 3; i++) {
      iss >> num >> dot;
      if (num < 0 || 255 < num || dot != '.') {
        nodeError("Error: 0 ~ 255 & only .");
      }
    }
    iss >> num;
    if (num < 0 || 255 < num || iss.eof() == false) {
      nodeError("Error: 0 ~ 255 & only .");
    }

    // Port validate
    for (std::string::size_type i = 0; i < portStr.size(); ++i) {
      if (!isdigit(portStr[i])) {
        nodeError("Error: Invalid port number.");
      }
    }

    if (port < 0 || 65535 < port) {
      nodeError("Error: 0 ~ 65535");
    }
  }
}

void Node::checkServerName(std::vector<std::string> &value) {
  if (value.size() < 1) {
    nodeError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Node::checkReturn(std::vector<std::string> &value) {
  if (value.size() != 2) {
    nodeError("Error: Incorrect number of arguments. Exactly two arguments are "
              "required.");
  }
  std::string firstValue = value[0];

  for (int i = 0; i < firstValue.size(); i++) {
    if (isdigit(firstValue[i]) == false) {
      nodeError("Error: Invalid character found in status code argument.");
    }
  }

  int num = atoi(firstValue.c_str());
  if (num < 0 || num > 999) {
    nodeError(
        "Error: The first value must be a number within the range 0-999.");
  }
}

void Node::checkAlias(std::vector<std::string> &value) {
  if (value.size() != 1) {
    nodeError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Node::checkLimitExcept(std::vector<std::string> &value) {
  if (value.size() < 1) {
    nodeError("Error:");
  }
  std::vector<std::string> validMethods;
  validMethods.push_back("GET");
  validMethods.push_back("POST");
  validMethods.push_back("DELETE");

  for (int i = 0; i < value.size(); i++) {
    if (std::find(validMethods.begin(), validMethods.end(), value[i]) ==
        validMethods.end()) {
      nodeError("Invalid method");
    }
  }
}
