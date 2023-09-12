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
           std::vector<std::string>::iterator &token, Node *parent, int level)
    : mParent(parent), mIsGood(true) {
  int tokenInfo;

  for (; token != configTokens.end(); token++) {
    tokenInfo = getTokenInfo(*token);

    if ((tokenInfo & level) == 0 && tokenInfo != CLOSE_BRACKET) {
      nodeError("Error: Invalid token level");
    }

    if (tokenInfo & CREATE_NODE) {
      bool bLocationFlag = false;
      std::string location;
      if (tokenInfo == LOCATION) { /* location */
        tokenInfo = getTokenInfo(*(++token));
        bLocationFlag = true;
        location = *token;
      }

      tokenInfo = getTokenInfo(*(++token));
      if ((tokenInfo & OPEN_BRACKET) == 0) {
        nodeError("Error: Bracket not opened");
      }

      Node *newNode = new Node(configTokens, ++token, this,
                               (level & LOCATION_LEVEL) ? level : level << 1);
      if (bLocationFlag) {
        newNode->mDirectives["location"].push_back(location);
      }
      mChildren.push_back(newNode);
    } else if (tokenInfo & OPEN_BRACKET) {
      nodeError("Error: Wrong bracket");
    } else if (tokenInfo & CLOSE_BRACKET) {
      if (level == NONE_LEVEL && (token != configTokens.end())) {
        nodeError("Error: Wrong bracket");
      }
      return;
    } else {
      addDirective(configTokens, token);
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
                        std::vector<std::string>::iterator &token) {
  std::string key = *token;

  if (mDirectives.find(key) != mDirectives.end()) {
    nodeError("Error: Directive already exists");
  }
  std::vector<std::string> values;
  token++;
  for (; token != configTokens.end(); token++) {
    int tokenInfo = getTokenInfo(*token);
    if (tokenInfo & SEMICOLON) {
      break;
    }
    values.push_back(*token);
  }
  if (token == configTokens.end()) {
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

void Node::nodeError(const std::string &msg)
{
  std::cerr << msg << std::endl;
  mIsGood = false;
}





void CheckErrorPage(std::vector<std::string> &value) {
  if (value.size() < 2) {
    std::cerr << "Error: Insufficient arguments provided. Please provide at "
                 "least two arguments."
              << std::endl;
      // return or ~
  }

  for (std::vector<std::string>::iterator it = value.begin();
       it != value.end() - 1; it++) {
    for (std::string::iterator strIt = it->begin(); strIt != it->end();
         ++strIt) {
      if (isdigit(*strIt) == false) {
        std::cerr << "Error: Invalid character found in status code argument."
                  << std::endl;
      // return or ~
      }
    }
    int statusCode = std::atoi(it->c_str());
    if (statusCode < 300 || statusCode > 599) {
      std::cerr << "Error: Status code out of valid range (300-599)."
                << std::endl;
      // return or ~
    }
  }
  // 마지막값은 유효성 검사하지 않아도 됨
}

void CheckClientMaxBodySize(std::vector<std::string> &value) {
  if (value.size() != 1) {
    std::cerr << "The number of values is not exactly 1!" << std::endl;
      // return or ~
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
      std::cerr << "Invalid" << std::endl;
      // return or ~
    }
  }

  int num = atoi(value[0].c_str());

  if (num < 0) {
    std::cerr << "Only positive numbers are possible" << std::endl;
      // return or ~
  }
  // mClientMaxBodySize = (num * multiplier);
}

void CheckIndex(std::vector<std::string> &value) {
  if (value.size() < 1) {
    std::cerr
        << "Error: No arguments provided. At least one argument is required."
        << std::endl;
      // return or ~
  }
}

void CheckAutoIndex(std::vector<std::string> &value) {

  if (value.size() != 1) {
    std::cerr << "The number of values is not exactly 1!" << std::endl;
      // return or ~
  }
  std::string upperName = value[0];
  std::transform(upperName.begin(), upperName.end(), upperName.begin(),
                 ::toupper);
  if (value[0] != "ON" && value[0] != "OFF") {
    std::cerr << "The value is not ON or OFF!" << std::endl;
      // return or ~
  }
}

void CheckListen(std::vector<std::string> &value)
{
	if (value.size() < 1)
	{
		std::cerr << "Error: No arguments provided. At least one argument is required." << std::endl;
		// return or ~
	}

	size_t pos = value[0].find(':');
	// only IP or only Port

	if (pos == std::string::npos)
	{
		if(value[0].find('.') != std::string::npos)
		{
			// Only IP
			std::istringstream iss(value[0]);
			int num;
			char dot;
			for (int i = 0; i < 3; i++)
			{
				iss >> num >> dot;
				if (num < 0 || 255 < num || dot != '.')
				{
					std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
					// return or ~
				}
			}
			iss >> num;
			if (num < 0 || 255 < num || iss.eof() == false)
			{
				std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
				// return or ~
			}
		}
		else
		{
			// Only Port
			for (int i = 0; i < value[0].size(); i++)
			{
				if (!isdigit(value[0][i]))
				{
					std::cerr << "Invalid" << std::endl;
					// return or ~
				}
			}
			int port = atoi(value[0].c_str());
			if (port < 0 || 65535 < port)
			{
				std::cerr << "Error: 0 ~ 65535" << std::endl;
				// return or ~
			}
		}
	}
	else
	{
		// IP:Port 127.0.0.1:8080
		std::string ip = value[0].substr(0, pos); // ex 127.0.0.1
		std::string portStr = value[0].substr(pos + 1); // 8080
		int port = atoi(portStr.c_str()); //8080

		// IP validate
		int num;
		char dot;
		std::istringstream iss(ip);

		for (int i = 0; i < 3; i++)
		{
			iss >> num >> dot;
			if (num < 0 || 255 < num || dot != '.')
			{
				std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
				// return or ~
			}
		}
		iss >> num;
		if (num < 0 || 255 < num || iss.eof() == false)
		{
			std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
			// return or ~
		}

		// Port validate
		for(std::string::size_type i = 0; i < portStr.size(); ++i)
		{
			if(!isdigit(portStr[i]))
			{
				std::cerr << "Error: Invalid port number." << std::endl;
				// return or ~
			}
		}

		if (port < 0 || 65535 < port)
		{
			std::cerr << "Error: 0 ~ 65535" << std::endl;
			// return or ~
		}
	}
}



// void CheckListen(std::vector<std::string> &value) {
// 	if (value.size() < 1)
// 	{
// 	std::cerr << "Error: No arguments provided. At least one argument is required." << std::endl;
// 		// return or ~
// 	}
// 	size_t pos = value[0].find(':');
// 	if (pos == std::string::npos)
// 	{
// 		// only Port
// 		for (int i = 0; i < value[0].size(); i++)
// 		{
// 			if (isdigit(value[0][i] == false))
// 			{
// 				std::cerr << "Invalid" << std::endl;
// 				// return or ~
// 			}
// 		}
// 		int port = atoi(value[0].c_str());
// 		if (port < 0 || 65535 < port)
// 		{
// 			std::cerr << "Error: 0 ~ 65535" << std::endl;
// 		}
// 	}
// 	else
// 	{
// 		// IP:Port 127.0.0.1:8080
// 		std::string ip = value[0].substr(0, pos); // ex 127.0.0.1
// 		std::string portStr = value[0].substr(pos + 1); // 8080
// 		int port = atoi(portStr.c_str()); //8080

// 		// IP validate
// 		int num;
// 		char dot;
// 		std::istringstream iss(ip);

// 		for (int i = 0; i < 3; i++)
// 		{
// 			iss >> num >> dot;
// 			if (num < 0 || 255 < num || dot != '.')
// 			{
// 				std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
// 				// return or ~
// 			}
// 		}
// 		iss >> num;
// 		if (num < 0 || 255 < num || iss.eof() == false)
// 		{
// 			std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
// 			// return or ~
// 		}
// 	}
// }

void CheckServerName(std::vector<std::string> &value)
{
  if (value.size() < 1) {
    std::cerr
        << "Error: No arguments provided. At least one argument is required."
        << std::endl;
      // return or ~
  }
}


void CheckReturn(std::vector<std::string> &value)
{
	if (value.size() != 2)
	{
    	std::cerr << "Error: Incorrect number of arguments. Exactly two arguments are required." << std::endl;
     	 // return or ~
	}
    std::string firstValue = value[0];

    for (int i = 0; i < firstValue.size(); i++)
    {
        if (isdigit(firstValue[i]) == false)
        {
        	std::cerr << "Error: Invalid character found in status code argument." << std::endl;
      		// return or ~
        }
    }

	int num = atoi(firstValue.c_str());
	if (num < 0 || num > 999)
	{
		std::cerr << "Error: The first value must be a number within the range 0-999." << std::endl;
	    // return or ~
	}
}

void Node::checkSum(int level)
{
  for (std::map<std::string, std::vector<std::string> >::iterator it =
           mDirectives.begin();
       it != mDirectives.end(); ++it) {
     std::cout << it->first << std::endl;
    if (it->first == "error_page")
      CheckErrorPage(it->second);
    else if (it->first == "client_max_body_size")
      CheckClientMaxBodySize(it->second);
    else if (it->first == "index")
      CheckIndex(it->second);
    else if (it->first == "autoindex")
      CheckAutoIndex(it->second);
    else if (it->first == "listen")
      CheckListen(it->second);
    else if (it->first == "server_name")
      CheckServerName(it->second);
    else if (it->first == "return")
      CheckReturn(it->second);
    // else if (it->first == "alias")
    //   CheckAlias();
    // else if (it->first == "limit_except")
    //   CheckLimitExcept();
  }
  for (std::vector<Node *>::iterator it = mChildren.begin();
       it != mChildren.end(); ++it) {
    (*it)->checkSum(level + 1);
       }
}
