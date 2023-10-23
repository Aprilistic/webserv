#include "Config.hpp"
#include "Tokenizer.hpp"

void Config::MakeConfigTree(const std::string &path) {

  try {
    std::vector<std::string> configTokens = Tokenizer::makeTokens(path);
    std::vector<std::string>::iterator tokenLocation = configTokens.begin();
    Common::mConfigTree = new Node(configTokens, tokenLocation, NULL, 1);
    checkSum(Common::mConfigTree);
  } catch (std::exception &e) {
    delete (Common::mConfigTree);
    Common::mConfigTree = NULL;
    std::cerr << e.what() << std::endl;
  }
}

void Config::MakeConfigMap() {
  try {
    Common::mConfigMap = new ConfigMap(Common::mConfigTree);
  } catch (std::exception &e) {
    Common::mConfigMap = NULL;
    std::cerr << e.what() << std::endl;
  }
}

void Config::checkSum(Node *configTree) {
  for (std::map<std::string, std::vector<std::string> >::iterator it =
           configTree->mDirectives.begin();
       it != configTree->mDirectives.end(); ++it) {
    if (it->first == "error_page")
      checkErrorPage(it->second);
    else if (it->first == "client_max_body_size")
      checkConnectionMaxBodySize(it->second);
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
  for (std::vector<Node *>::iterator it = configTree->mChildren.begin();
       it != configTree->mChildren.end(); ++it) {
    checkSum(*it);
  }
}

std::map<int, Server *> Config::MakeServerList() {
  std::map<int, Server *> ServerList;
  try {
    std::vector<int> ports = Common::mConfigMap->GetPorts();
    for (size_t i = 0; i < ports.size(); i++) {
      Server *newServer = new Server(ports[i]);
      ServerList[ports[i]] = newServer;
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    for (std::map<int, Server *>::iterator it = ServerList.begin();
         it != ServerList.end(); ++it) {
      delete it->second;
    }
    ServerList.clear();
  }

  return (ServerList);
}

void Config::checkErrorPage(std::vector<std::string> &value) {
  if (value.size() < 2) {
    configError("Error: Incorrect number of arguments. At least two arguments "
                "are required.");
  }
  for (std::vector<std::string>::iterator it = value.begin();
       it != value.end() - 1; it++) {
    for (std::string::iterator strIt = it->begin(); strIt != it->end();
         ++strIt) {
      if (isdigit(*strIt) == false) {
        configError("Error: Invalid character found in status code argument.");
      }
    }
    int statusCode = std::atoi(it->c_str());
    if (statusCode < 300 || statusCode > 599) {
      configError("Error: Status code out of valid range (300-599).");
    }
  }
  // 마지막값은 유효성 검사하지 않아도 됨
}

void Config::checkConnectionMaxBodySize(std::vector<std::string> &value) {
  if (value.size() != 1) {
    configError("Error: Incorrect number of arguments. Exactly one argument is "
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
      configError("Error: Invalid character found in status code argument.");
    }
  }

  int num = std::atoi(value[0].c_str());

  if (num < 0) {
    configError("Error: Only positive numbers are possible.");
  }
  // mConnectionMaxBodySize = (num * multiplier);
}

void Config::checkIndex(std::vector<std::string> &value) {
  if (value.size() < 1) {
    configError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Config::checkAutoIndex(std::vector<std::string> &value) {

  if (value.size() != 1) {
    configError("Error: Incorrect number of arguments. Exactly one argument is "
                "required.");
  }
  std::string upperName = value[0];
  std::transform(upperName.begin(), upperName.end(), upperName.begin(),
                 ::toupper);
  if (upperName != "ON" && upperName != "OFF") {
    configError("The value is not ON or OFF!");
  }
}

void Config::checkListen(std::vector<std::string> &value) {
  if (value.size() < 1) {
    configError(
        "Error: No arguments provided. At least one argument is required.");
  }
  if (value.size() == 2) {
    if (value[1] != "default_server") {
      configError("Error: hi yim");
    }
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
          configError("Error: 0 ~ 255 & only .");
        }
      }
      iss >> num;
      if (num < 0 || 255 < num || iss.eof() == false) {
        configError("Error: 0 ~ 255 & only .");
      }
    } else {
      // Only Port
      for (int i = 0; i < value[0].size(); i++) {
        if (!isdigit(value[0][i])) {
          configError("Error: Invalid port number.");
        }
      }
      int port = atoi(value[0].c_str());
      if (port < 0 || 65535 < port) {
        configError("Error: 0 ~ 65535");
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
        configError("Error: 0 ~ 255 & only .");
      }
    }
    iss >> num;
    if (num < 0 || 255 < num || iss.eof() == false) {
      configError("Error: 0 ~ 255 & only .");
    }

    // Port validate
    for (std::string::size_type i = 0; i < portStr.size(); ++i) {
      if (!isdigit(portStr[i])) {
        configError("Error: Invalid port number.");
      }
    }

    if (port < 0 || 65535 < port) {
      configError("Error: 0 ~ 65535");
    }
  }
}

void Config::checkServerName(std::vector<std::string> &value) {
  if (value.size() < 1) {
    configError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Config::checkReturn(std::vector<std::string> &value) {
  if (value.size() != 2) {
    configError(
        "Error: Incorrect number of arguments. Exactly two arguments are "
        "required.");
  }
  std::string firstValue = value[0];

  for (int i = 0; i < firstValue.size(); i++) {
    if (isdigit(firstValue[i]) == false) {
      configError("Error: Invalid character found in status code argument.");
    }
  }

  int num = atoi(firstValue.c_str());
  if (num < 0 || num > 999) {
    configError(
        "Error: The first value must be a number within the range 0-999.");
  }
}

void Config::checkAlias(std::vector<std::string> &value) {
  if (value.size() != 1) {
    configError(
        "Error: No arguments provided. At least one argument is required.");
  }
}

void Config::checkLimitExcept(std::vector<std::string> &value) {
  if (value.size() < 1) {
    configError("Error:");
  }
  std::vector<std::string> validMethods;
  validMethods.push_back("GET");
  validMethods.push_back("POST");
  validMethods.push_back("DELETE");

  for (int i = 0; i < value.size(); i++) {
    if (std::find(validMethods.begin(), validMethods.end(), value[i]) ==
        validMethods.end()) {
      configError("Invalid method");
    }
  }
}

void Config::configError(const std::string &msg) {
  throw std::runtime_error(msg);
}
