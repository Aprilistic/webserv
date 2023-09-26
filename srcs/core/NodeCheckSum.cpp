#include "Node.hpp"

void Node::checkSum(int level) {
  for (std::map<std::string, std::vector<std::string> >::iterator it =
           mDirectives.begin();
       it != mDirectives.end(); ++it) {
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
    else if (it->first == "alias")
      CheckAlias(it->second);
    else if (it->first == "limit_except")
      CheckLimitExcept(it->second);
  }
  for (std::vector<Node *>::iterator it = mChildren.begin();
       it != mChildren.end(); ++it) {
    (*it)->checkSum(level + 1);
  }
}

void Node::CheckErrorPage(std::vector<std::string> &value) {
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

void Node::CheckClientMaxBodySize(std::vector<std::string> &value) {
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

void Node::CheckIndex(std::vector<std::string> &value) {
  if (value.size() < 1) {
    nodeError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Node::CheckAutoIndex(std::vector<std::string> &value) {

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

void Node::CheckListen(std::vector<std::string> &value) {
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

void Node::CheckServerName(std::vector<std::string> &value) {
  if (value.size() < 1) {
    nodeError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Node::CheckReturn(std::vector<std::string> &value) {
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

void Node::CheckAlias(std::vector<std::string> &value) {
  if (value.size() != 1) {
    nodeError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Node::CheckLimitExcept(std::vector<std::string> &value) {
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
